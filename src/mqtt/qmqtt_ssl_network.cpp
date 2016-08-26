/*
 * qmqtt_ssl_network.cpp - qmqtt SSL network
 *
 * Copyright (c) 2013  Ery Lee <ery.lee at gmail dot com>
 * Copyright (c) 2016  Matthias Dieter Wallnöfer
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of mqttc nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <QDataStream>
#include "qmqtt_ssl_network_p.h"
#include "qmqtt_ssl_socket_p.h"
#include "qmqtt_timer_p.h"

#ifndef QT_NO_SSL

const QString DEFAULT_HOST_NAME = QStringLiteral("localhost");
const quint16 DEFAULT_PORT = 8883;
const bool DEFAULT_AUTORECONNECT = false;
const int DEFAULT_AUTORECONNECT_INTERVAL_MS = 5000;

QMQTT::SslNetwork::SslNetwork(bool ignoreSelfSigned, QObject* parent)
    : NetworkInterface(parent)
    , _port(DEFAULT_PORT)
    , _hostName(DEFAULT_HOST_NAME)
    , _autoReconnect(DEFAULT_AUTORECONNECT)
    , _autoReconnectInterval(DEFAULT_AUTORECONNECT_INTERVAL_MS)
    , _bytesRemaining(0)
    , _socket(new QMQTT::SslSocket(ignoreSelfSigned))
    , _autoReconnectTimer(new QMQTT::Timer)
{
    initialize();
}

QMQTT::SslNetwork::SslNetwork(SocketInterface* socketInterface, TimerInterface* timerInterface,
                        QObject* parent)
    : NetworkInterface(parent)
    , _port(DEFAULT_PORT)
    , _hostName(DEFAULT_HOST_NAME)
    , _autoReconnect(DEFAULT_AUTORECONNECT)
    , _autoReconnectInterval(DEFAULT_AUTORECONNECT_INTERVAL_MS)
    , _bytesRemaining(0)
    , _socket(socketInterface)
    , _autoReconnectTimer(timerInterface)
{
    initialize();
}

void QMQTT::SslNetwork::initialize()
{
    _socket->setParent(this);
    _autoReconnectTimer->setParent(this);
    _autoReconnectTimer->setSingleShot(true);
    _autoReconnectTimer->setInterval(_autoReconnectInterval);

    QObject::connect(_socket, &SocketInterface::connected, this, &SslNetwork::connected);
    QObject::connect(_socket, &SocketInterface::disconnected, this, &SslNetwork::onDisconnected);
    QObject::connect(_socket->ioDevice(), &QIODevice::readyRead, this, &SslNetwork::onSocketReadReady);
    QObject::connect(
        _autoReconnectTimer, &TimerInterface::timeout,
        this, static_cast<void (SslNetwork::*)()>(&SslNetwork::connectToHost));
    QObject::connect(_socket,
        static_cast<void (SocketInterface::*)(QAbstractSocket::SocketError)>(&SocketInterface::error),
        this, &SslNetwork::onSocketError);
}

QMQTT::SslNetwork::~SslNetwork()
{
}

bool QMQTT::SslNetwork::isConnectedToHost() const
{
    return _socket->state() == QAbstractSocket::ConnectedState;
}

void QMQTT::SslNetwork::connectToHost(const QHostAddress& host, const quint16 port)
{
    Q_UNUSED(host);
    Q_UNUSED(port);

    qCritical("qmqtt: SSL does not work with host addresses!");
    emit error(QAbstractSocket::ConnectionRefusedError);
}

void QMQTT::SslNetwork::connectToHost(const QString& hostName, const quint16 port)
{
    _hostName = hostName;
    _port = port;
    connectToHost();
}

void QMQTT::SslNetwork::connectToHost()
{
    _bytesRemaining = 0;
    _socket->connectToHost(_hostName, _port);
}

void QMQTT::SslNetwork::onSocketError(QAbstractSocket::SocketError socketError)
{
    emit error(socketError);
    if(_autoReconnect)
    {
        _autoReconnectTimer->start();
    }
}

void QMQTT::SslNetwork::sendFrame(Frame& frame)
{
    if(_socket->state() == QAbstractSocket::ConnectedState)
    {
        QDataStream out(_socket->ioDevice());
        frame.write(out);
    }
}

void QMQTT::SslNetwork::disconnectFromHost()
{
    _socket->disconnectFromHost();
}

QAbstractSocket::SocketState QMQTT::SslNetwork::state() const
{
    return _socket->state();
}

bool QMQTT::SslNetwork::autoReconnect() const
{
    return _autoReconnect;
}

void QMQTT::SslNetwork::setAutoReconnect(const bool autoReconnect)
{
    _autoReconnect = autoReconnect;
}

int QMQTT::SslNetwork::autoReconnectInterval() const
{
    return _autoReconnectInterval;
}

void QMQTT::SslNetwork::setAutoReconnectInterval(const int autoReconnectInterval)
{
    _autoReconnectInterval = autoReconnectInterval;
}

void QMQTT::SslNetwork::onSocketReadReady()
{
    QIODevice *ioDevice = _socket->ioDevice();
    while(!ioDevice->atEnd())
    {
        if(_bytesRemaining == 0)
        {
            if (!ioDevice->getChar(reinterpret_cast<char *>(&_header)))
            {
                // malformed packet
                emit error(QAbstractSocket::OperationError);
                ioDevice->close();
                return;
            }

            _bytesRemaining = readRemainingLength();
            if (_bytesRemaining < 0)
            {
                // malformed remaining length
                emit error(QAbstractSocket::OperationError);
                ioDevice->close();
                return;
            }
        }

        QByteArray data = ioDevice->read(_bytesRemaining);
        _buffer.append(data);
        _bytesRemaining -= data.size();

        if(_bytesRemaining == 0)
        {
            Frame frame(_header, _buffer);
            _buffer.clear();
            emit received(frame);
        }
    }
}

int QMQTT::SslNetwork::readRemainingLength()
{
     quint8 byte = 0;
     int length = 0;
     int multiplier = 1;
     QIODevice *ioDevice = _socket->ioDevice();
     do {
         if (!ioDevice->getChar(reinterpret_cast<char *>(&byte)))
             return -1;
         length += (byte & 127) * multiplier;
         multiplier *= 128;
         if (multiplier > 128*128*128)
             return -1;
     } while ((byte & 128) != 0);

     return length;
}

void QMQTT::SslNetwork::onDisconnected()
{
    emit disconnected();
    if(_autoReconnect)
    {
        _autoReconnectTimer->start();
    }
}

#endif // QT_NO_SSL
