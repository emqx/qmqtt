/*
 * qmqtt_client_p.cpp - qmqtt client private
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

#include "qmqtt_client_p.h"
#include "qmqtt_message.h"
#include <QLoggingCategory>
#include <QDateTime>
#include <QUuid>

Q_LOGGING_CATEGORY(client, "qmqtt.client")

static const quint8 QOS0 = 0x00;
static const quint8 QOS1 = 0x01;
static const quint8 QOS2 = 0x02;

QMQTT::ClientPrivate::ClientPrivate(Client* qq_ptr)
    : _host(QHostAddress::LocalHost)
    , _port(1883)
    , _gmid(1)
    , _clientId(QUuid::createUuid().toString())
    , _cleanSession(false)
    , _keepAlive(300)
    , _connectionState(STATE_INIT)
    , _willQos(0)
    , _willRetain(false)
    , q_ptr(qq_ptr)
{
}

QMQTT::ClientPrivate::~ClientPrivate()
{
}

void QMQTT::ClientPrivate::init(const QHostAddress& host, const quint16 port, NetworkInterface* network)
{
    Q_Q(Client);

    _host = host;
    _port = port;
    if(NULL == network)
    {
        _network.reset(new Network);
    }
    else
    {
        _network.reset(network);
    }

    QObject::connect(&_timer, &QTimer::timeout, q, &Client::onTimerPingReq);

    QObject::connect(_network.data(), &Network::connected,
                     q, &Client::onNetworkConnected);
    QObject::connect(_network.data(), &Network::disconnected,
                     q, &Client::onNetworkDisconnected);
    QObject::connect(_network.data(), &Network::received,
                     q, &Client::onNetworkReceived);
}

void QMQTT::ClientPrivate::connectToHost()
{
    _network->connectToHost(_host, _port);
}

void QMQTT::ClientPrivate::onNetworkConnected()
{
    Q_Q(Client);
    sendConnect();
    startKeepAlive();
    emit q->connected();
}

void QMQTT::ClientPrivate::sendConnect()
{
    QString magic(PROTOCOL_MAGIC);
    quint8 header = CONNECT;
    quint8 flags = 0;

    //header
    Frame frame(SETQOS(header, QOS1));

    //flags
    flags = FLAG_CLEANSESS(flags, _cleanSession ? 1 : 0 );
    flags = FLAG_WILL(flags, willTopic().isEmpty() ? 0 : 1);
    if (!willTopic().isEmpty())
    {
        flags = FLAG_WILLQOS(flags, willQos());
        flags = FLAG_WILLRETAIN(flags, willRetain() ? 1 : 0);
    }
    if (!username().isEmpty())
    {
        flags = FLAG_USERNAME(flags, 1);
    }
    if (!password().isEmpty())
    {
        flags = FLAG_PASSWD(flags, 1);
    }

    //payload
    frame.writeString(magic);
    frame.writeChar(PROTOCOL_VERSION_MAJOR);
    frame.writeChar(flags);
    frame.writeInt(_keepAlive);
    if(_clientId.isEmpty()) {
        _clientId = randomClientId();
    }
    frame.writeString(_clientId);
    if(!willTopic().isEmpty())
    {
        frame.writeString(willTopic());
        if(!willMessage().isEmpty())
        {
            frame.writeString(willMessage());
        }
    }
    if (!_username.isEmpty())
    {
        frame.writeString(_username);
    }
    if (!_password.isEmpty()){
        frame.writeString(_password);
    }
    _network->sendFrame(frame);
}

quint16 QMQTT::ClientPrivate::sendPublish(const Message& msg)
{
    Message message(msg);

    quint8 header = PUBLISH;
    header = SETRETAIN(header, message.retain() ? 1 : 0);
    header = SETQOS(header, message.qos());
    header = SETDUP(header, message.dup() ? 1 : 0);
    Frame frame(header);
    frame.writeString(message.topic());
    if(message.qos() > QOS0) {
        if(message.id() == 0) {
            message.setId(_gmid++);
        }
        frame.writeInt(message.id());
    }
    if(!message.payload().isEmpty()) {
        frame.writeRawData(message.payload());
    }
    _network->sendFrame(frame);
    return message.id();
}

void QMQTT::ClientPrivate::sendPuback(quint8 type, quint16 mid)
{
    Frame frame(type);
    frame.writeInt(mid);
    _network->sendFrame(frame);
}

quint16 QMQTT::ClientPrivate::sendSubscribe(const QString & topic, quint8 qos)
{
    quint16 mid = nextmid();
    Frame frame(SETQOS(SUBSCRIBE, QOS1));
    frame.writeInt(mid);
    frame.writeString(topic);
    frame.writeChar(qos);
    _network->sendFrame(frame);
    return mid;
}

quint16 QMQTT::ClientPrivate::sendUnsubscribe(const QString &topic)
{
    quint16 mid = _gmid++;
    Frame frame(SETQOS(UNSUBSCRIBE, QOS1));
    frame.writeInt(mid);
    frame.writeString(topic);
    _network->sendFrame(frame);
    return mid;
}

void QMQTT::ClientPrivate::onTimerPingReq()
{
    Frame frame(PINGREQ);
    _network->sendFrame(frame);
}

void QMQTT::ClientPrivate::disconnectFromHost()
{
    sendDisconnect();
    _network->disconnectFromHost();
}

void QMQTT::ClientPrivate::sendDisconnect()
{
    Frame frame(DISCONNECT);
    _network->sendFrame(frame);
}

void QMQTT::ClientPrivate::startKeepAlive()
{
    _timer.setInterval(_keepAlive*1000);
    _timer.start();
}

void QMQTT::ClientPrivate::stopKeepAlive()
{
    _timer.stop();
}

QString QMQTT::ClientPrivate::randomClientId()
{
    return "QMQTT-" + QString::number(QDateTime::currentMSecsSinceEpoch() % 1000000);
}

quint16 QMQTT::ClientPrivate::nextmid()
{
    return _gmid++;
}

quint16 QMQTT::ClientPrivate::publish(const Message& message)
{
    Q_Q(Client);
    quint16 msgid = sendPublish(message);
    emit q->published(message);
    return msgid;
}

void QMQTT::ClientPrivate::puback(const quint8 type, const quint16 msgid)
{
    sendPuback(type, msgid);
}

quint16 QMQTT::ClientPrivate::subscribe(const QString& topic, const quint8 qos)
{
    Q_Q(Client);
    quint16 msgid = sendSubscribe(topic, qos);
    emit q->subscribed(topic);
    return msgid;
}

void QMQTT::ClientPrivate::unsubscribe(const QString& topic)
{
    Q_Q(Client);
    sendUnsubscribe(topic);
    emit q->unsubscribed(topic);
}

void QMQTT::ClientPrivate::onNetworkDisconnected()
{
    Q_Q(Client);

    stopKeepAlive();
    emit q->disconnected();
}

void QMQTT::ClientPrivate::onNetworkReceived(const QMQTT::Frame& frm)
{
    QMQTT::Frame frame(frm);
    quint8 qos = 0;
    bool retain, dup;
    QString topic;
    quint16 mid = 0;
    quint8 header = frame.header();
    quint8 type = GETTYPE(header);
    Message message;

    switch(type)
    {
    case CONNACK:
        frame.readChar();
        handleConnack(frame.readChar());
        break;
    case PUBLISH:
        qos = GETQOS(header);;
        retain = GETRETAIN(header);
        dup = GETDUP(header);
        topic = frame.readString();
        if( qos > QOS0) {
            mid = frame.readInt();
        }
        message.setId(mid);
        message.setTopic(topic);
        message.setPayload(frame.data());
        message.setQos(qos);
        message.setRetain(retain);
        message.setDup(dup);
        handlePublish(message);
        break;
    case PUBACK:
    case PUBREC:
    case PUBREL:
    case PUBCOMP:
        mid = frame.readInt();
        handlePuback(type, mid);
        break;
    case SUBACK:
        mid = frame.readInt();
        qos = frame.readChar();
        // todo: send a subscribed signal (only in certain cases? mid? qos?)
        break;
    case UNSUBACK:
        // todo: send an unsubscribed signal (only certain cases? mid?)
        break;
    case PINGRESP:
        // todo: I know I'm suppose to do something with this. Look at specifications.
        break;
    default:
        break;
    }
}

void QMQTT::ClientPrivate::handleConnack(const quint8 ack)
{
    Q_UNUSED(ack);
    // todo: send connected signal
}

void QMQTT::ClientPrivate::handlePublish(const Message& message)
{
    Q_Q(Client);

    if(message.qos() == QOS1)
    {
        sendPuback(PUBACK, message.id());
    }
    else if(message.qos() == QOS2)
    {
        sendPuback(PUBREC, message.id());
    }
    emit q->received(message);
}

void QMQTT::ClientPrivate::handlePuback(const quint8 type, const quint16 msgid)
{
    if(type == PUBREC)
    {
        sendPuback(PUBREL, msgid);
    }
    else if (type == PUBREL)
    {
        sendPuback(PUBCOMP, msgid);
    }
    // todo: emit published signal (type? msgid?)
}

bool QMQTT::ClientPrivate::autoReconnect() const
{
    return _network->autoReconnect();
}

void QMQTT::ClientPrivate::setAutoReconnect(const bool value)
{
    _network->setAutoReconnect(value);
}

bool QMQTT::ClientPrivate::isConnectedToHost() const
{
    return _network->isConnectedToHost();
}

QMQTT::ConnectionState QMQTT::ClientPrivate::connectionState() const
{
    return _connectionState;
}

void QMQTT::ClientPrivate::setCleanSession(const bool cleanSession)
{
    _cleanSession = cleanSession;
}

bool QMQTT::ClientPrivate::cleanSession() const
{
    return _cleanSession;
}

void QMQTT::ClientPrivate::setKeepAlive(const int keepAlive)
{
    _keepAlive = keepAlive;
}

int QMQTT::ClientPrivate::keepAlive() const
{
    return _keepAlive;
}

void QMQTT::ClientPrivate::setPassword(const QString& password)
{
    _password = password;
}

QString QMQTT::ClientPrivate::password() const
{
    return _password;
}

void QMQTT::ClientPrivate::setUsername(const QString& username)
{
    _username = username;
}

QString QMQTT::ClientPrivate::username() const
{
    return _username;
}

void QMQTT::ClientPrivate::setClientId(const QString& clientId)
{
    if(clientId.isEmpty())
    {
        _clientId = QUuid::createUuid().toString();
    }
    else
    {
        _clientId = clientId;
    }
}

QString QMQTT::ClientPrivate::clientId() const
{
    return _clientId;
}

void QMQTT::ClientPrivate::setPort(const quint16 port)
{
    _port = port;
}

quint16 QMQTT::ClientPrivate::port() const
{
    return _port;
}

void QMQTT::ClientPrivate::setHost(const QHostAddress& host)
{
    _host = host;
}

QHostAddress QMQTT::ClientPrivate::host() const
{
    return _host;
}

QString QMQTT::ClientPrivate::willTopic() const
{
    return _willTopic;
}

void QMQTT::ClientPrivate::setWillTopic(const QString& willTopic)
{
    _willTopic = willTopic;
}

quint8 QMQTT::ClientPrivate::willQos() const
{
    return _willQos;
}

void QMQTT::ClientPrivate::setWillQos(const quint8 willQos)
{
    _willQos = willQos;
}

bool QMQTT::ClientPrivate::willRetain() const
{
    return _willRetain;
}

void QMQTT::ClientPrivate::setWillRetain(const bool willRetain)
{
    _willRetain = willRetain;
}

QString QMQTT::ClientPrivate::willMessage() const
{
    return _willMessage;
}

void QMQTT::ClientPrivate::setWillMessage(const QString& willMessage)
{
    _willMessage = willMessage;
}
