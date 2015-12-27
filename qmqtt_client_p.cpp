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

Q_LOGGING_CATEGORY(client, "qmqtt.client")

static const quint8 QOS0 = 0x00;
static const quint8 QOS1 = 0x01;
static const quint8 QOS2 = 0x02;

QMQTT::ClientPrivate::ClientPrivate(Client* qq_ptr)
    : _host("localhost")
    , _port(1883)
    , _gmid(1)
    , _cleansess(false)
    , _keepalive(300)
    , _state(STATE_INIT)
    , _will(NULL)
    , q_ptr(qq_ptr)
{
}

QMQTT::ClientPrivate::~ClientPrivate()
{
    if(NULL != _will)
    {
        delete _will;
        _will = NULL;
    }
}

void QMQTT::ClientPrivate::init(const QString& host, const quint16 port)
{
    Q_Q(Client);
    _host = host;
    _port = port;
    QObject::connect(&_timer, SIGNAL(timeout()), q, SLOT(ping()));
    //TODO: FIXME LATER, how to handle socket error?
    QObject::connect(&_network, SIGNAL(connected()), q, SLOT(onNetworkConnected()));
    QObject::connect(&_network, SIGNAL(error(QAbstractSocket::SocketError)), q, SIGNAL(error(QAbstractSocket::SocketError)));
    QObject::connect(&_network, SIGNAL(disconnected()), q, SLOT(onNetworkDisconnected()));
    QObject::connect(&_network, SIGNAL(received(const QMQTT::Frame&)), q, SLOT(onReceived(const QMQTT::Frame&)));
}

void QMQTT::ClientPrivate::connectToHost()
{
    _network.connectToHost(_host, _port);
}

void QMQTT::ClientPrivate::onNetworkConnected()
{
    Q_Q(Client);
    qCDebug(client) << "Sock Connected....";
    sendConnect();
    startKeepalive();
    emit q->connected();
}

void QMQTT::ClientPrivate::sendConnect()
{
    QString magic(PROTOCOL_MAGIC);
    quint8 header = CONNECT;
    quint8 flags = 0;

    qCDebug(client) << "sendConnect....";

    //header
    Frame frame(SETQOS(header, QOS1));

    qCDebug(client) << "CONNECT header: " << frame.header();

    //flags
    flags = FLAG_CLEANSESS(flags, _cleansess ? 1 : 0 );
    flags = FLAG_WILL(flags, _will ? 1 : 0);
    if (_will) {
        flags = FLAG_WILLQOS(flags, _will->qos());
        flags = FLAG_WILLRETAIN(flags, _will->retain() ? 1 : 0);
    }
    if (!_username.isEmpty()) {
        flags = FLAG_USERNAME(flags, 1);
    }
    if (!_password.isEmpty()) {
        flags = FLAG_PASSWD(flags, 1);
    }

    //payload
    frame.writeString(magic);
    frame.writeChar(PROTOCOL_VERSION_MAJOR);
    frame.writeChar(flags);
    frame.writeInt(_keepalive);
    if(_clientId.isEmpty()) {
        _clientId = randomClientId();
    }
    frame.writeString(_clientId);
    if(_will != NULL) {
        frame.writeString(_will->topic());
        frame.writeString(_will->message());
    }
    if (!_username.isEmpty()) {
        frame.writeString(_username);
    }
    if (!_password.isEmpty()) {
        frame.writeString(_password);
    }
    _network.sendFrame(frame);
}

quint16 QMQTT::ClientPrivate::sendPublish(Message &msg)
{
    quint8 header = PUBLISH;
    header = SETRETAIN(header, msg.retain() ? 1 : 0);
    header = SETQOS(header, msg.qos());
    header = SETDUP(header, msg.dup() ? 1 : 0);
    Frame frame(header);
    frame.writeString(msg.topic());
    if(msg.qos() > QOS0) {
        if(msg.id() == 0) {
            msg.setId(_gmid++);
        }
        frame.writeInt(msg.id());
    }
    if(!msg.payload().isEmpty()) {
        frame.writeRawData(msg.payload());
    }
    _network.sendFrame(frame);
    return msg.id();
}

void QMQTT::ClientPrivate::sendPuback(quint8 type, quint16 mid)
{
    Frame frame(type);
    frame.writeInt(mid);
    _network.sendFrame(frame);
}

quint16 QMQTT::ClientPrivate::sendSubscribe(const QString & topic, quint8 qos)
{
    quint16 mid = nextmid();
    Frame frame(SETQOS(SUBSCRIBE, QOS1));
    frame.writeInt(mid);
    frame.writeString(topic);
    frame.writeChar(qos);
    _network.sendFrame(frame);
    return mid;
}

quint16 QMQTT::ClientPrivate::sendUnsubscribe(const QString &topic)
{
    quint16 mid = _gmid++;
    Frame frame(SETQOS(UNSUBSCRIBE, QOS1));
    frame.writeInt(mid);
    frame.writeString(topic);
    _network.sendFrame(frame);
    return mid;
}

void QMQTT::ClientPrivate::ping()
{
    Frame frame(PINGREQ);
    _network.sendFrame(frame);
}

void QMQTT::ClientPrivate::disconnectFromHost()
{
    sendDisconnect();
    _network.disconnectFromHost();
}

void QMQTT::ClientPrivate::sendDisconnect()
{
    Frame frame(DISCONNECT);
    _network.sendFrame(frame);
}

//FIXME: ok???
void QMQTT::ClientPrivate::startKeepalive()
{
    _timer.setInterval(_keepalive*1000);
    _timer.start();
}

void QMQTT::ClientPrivate::stopKeepalive()
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

quint16 QMQTT::ClientPrivate::publish(Message& message)
{
    Q_Q(Client);
    quint16 msgid = sendPublish(message);
    emit q->published(message);
    return msgid;
}

void QMQTT::ClientPrivate::puback(const quint8 type, const quint16 msgid)
{
    Q_Q(Client);
    sendPuback(type, msgid);
    emit q->pubacked(type, msgid);
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

    stopKeepalive();
    emit q->disconnected();
}

void QMQTT::ClientPrivate::onReceived(const QMQTT::Frame& frm)
{
    Q_Q(Client);

    QMQTT::Frame frame(frm);
    quint8 qos = 0;
    bool retain, dup;
    QString topic;
    quint16 mid = 0;
    quint8 header = frame.header();
    quint8 type = GETTYPE(header);
    Message message;
    qCDebug(client) << "handleFrame: type=" << type;

    switch(type) {
    case CONNACK:
        //skip reserved
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
        emit q->subacked(mid, qos);
        break;
    case UNSUBACK:
        emit q->unsubacked(mid);
        break;
    case PINGRESP:
        emit q->pong();
        break;
    default:
        break;
    }
}

void QMQTT::ClientPrivate::handleConnack(const quint8 ack)
{
    Q_Q(Client);

    qCDebug(client) << "connack: " << ack;
    emit q->connacked(ack);
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
    Q_Q(Client);

    if(type == PUBREC)
    {
        sendPuback(PUBREL, msgid);
    }
    else if (type == PUBREL)
    {
        sendPuback(PUBCOMP, msgid);
    }
    emit q->pubacked(type, msgid);
}

bool QMQTT::ClientPrivate::autoReconnect() const
{
    return _network.autoReconnect();
}

void QMQTT::ClientPrivate::setAutoReconnect(const bool value)
{
    _network.setAutoReconnect(value);
}

bool QMQTT::ClientPrivate::isConnectedToHost() const
{
    return _network.isConnectedToHost();
}

QMQTT::Will* QMQTT::ClientPrivate::will() const
{
    return _will;
}

void QMQTT::ClientPrivate::setWill(Will* will)
{
    _will = will;
}

QMQTT::ClientState QMQTT::ClientPrivate::state() const
{
    return _state;
}

void QMQTT::ClientPrivate::setCleansess(const bool cleansess)
{
    _cleansess = cleansess;
}

bool QMQTT::ClientPrivate::cleansess()
{
    return _cleansess;
}

void QMQTT::ClientPrivate::setKeepAlive(const int keepalive)
{
    _keepalive = keepalive;
}

int QMQTT::ClientPrivate::keepalive() const
{
    return _keepalive;
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
    _clientId = clientId;
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

void QMQTT::ClientPrivate::setHost(const QString& host)
{
    _host = host;
}

QString QMQTT::ClientPrivate::host() const
{
    return _host;
}
