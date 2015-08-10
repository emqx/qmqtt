/*
 * qmqtt_network.cpp - qmqtt network
 *
 * Copyright (c) 2013  Ery Lee <ery.lee at gmail dot com>
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
#include <QLoggingCategory>
#include "qmqtt_network.h"

namespace QMQTT {

Q_LOGGING_CATEGORY(network, "qmqtt.network")

Network::Network(QObject *parent) :
    QObject(parent)
{
    _buffer = new QBuffer(this);
    _offsetBuf = 0;
    _leftSize = 0;
    _autoreconn = false;
    _timeout = 3000;
    _connected = false;
    _buffer->open(QIODevice::ReadWrite);
    initSocket();
}

void Network::initSocket()
{
    if(_socket) {
        _socket->abort();
        delete _socket;
    }

    _socket = new QTcpSocket(this);
    connect(_socket, SIGNAL(connected()), this, SLOT(sockConnected()));
    connect(_socket, SIGNAL(disconnected()), this, SLOT(sockDisconnected()));
    connect(_socket, SIGNAL(readyRead()), this, SLOT(sockReadReady()));
    connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SIGNAL(error(QAbstractSocket::SocketError)));
}

Network::~Network()
{
    disconnect();
}

bool Network::isConnected()
{
    return _connected;
}

void Network::connectTo(const QString & host, const quint32 port)
{

    if(!_socket)
    {
        qCWarning(network) << "AMQP: Socket didn't create.";
        return;
    }
    _host = host;
    _port = port;
    qCDebug(network) << "Connect to" << _host << _port;
    _socket->connectToHost(host, port);
}

void Network::sendFrame(Frame & frame)
{

    if(_socket->state() == QAbstractSocket::ConnectedState)
    {
        QDataStream out(_socket);
        frame.write(out);
    }

}

void Network::disconnect()
{
    if(_socket) _socket->close();
}

QAbstractSocket::SocketState Network::state() const
{
    if(_socket)
    {
        return _socket->state();
    } else {
        return QAbstractSocket::UnconnectedState;
    }
}

bool Network::autoReconnect() const
{
    return _autoreconn;
}

void Network::setAutoReconnect(bool b)
{
    _autoreconn = b;
}

//PRIVATE SLOTS
void Network::sockConnected()
{
    qCDebug(network) << "Network connected...";
    _connected = true;
    emit connected();
}

void Network::sockReadReady()
{
    qCDebug(network) << "sockReadReady...";
    QDataStream in(_socket);
    QDataStream out(_buffer);
    while(!_socket->atEnd())
    {
        if(_leftSize == 0)
        {
            _leftSize  = 0;
            _offsetBuf = 0;

            in >> _header;
            _leftSize = readRemaingLength(in);
        }
        QByteArray data;
        data.resize(_leftSize);
        _offsetBuf = in.readRawData(data.data(), data.size());
        _leftSize -= _offsetBuf;
        out.writeRawData(data.data(), _offsetBuf);
        if(_leftSize == 0)
        {
            _buffer->reset();
            Frame frame(_header, _buffer->buffer());
            _buffer->buffer().clear();
            qCDebug(network) << "network emit received(frame), header: " << _header;
            emit received(frame);
        }
    }
}

/*
 * Read remaining length
 */
int Network::readRemaingLength(QDataStream &in)
{
     qint8 byte;
     int len = 0;
     int mul = 1;
     do {
         in >> byte;
         len += (byte & 127) * mul;
         mul *= 128  ;
     } while ((byte & 128) != 0);
     return len;
}

void Network::sockDisconnected()
{
    _connected = false;
    emit disconnected();
}

} // namespace QMQTT
