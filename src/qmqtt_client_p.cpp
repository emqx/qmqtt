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
#include "qmqtt_connectpacket.h"
#include "qmqtt_publishpacket.h"
#include "qmqtt_disconnectpacket.h"
#include "qmqtt_unsubscribepacket.h"
#include "qmqtt_subscribepacket.h"
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
    if(network == NULL)
    {
        _network.reset(new Network);
    }
    else
    {
        _network.reset(network);
    }

    initializeErrorHash();

    QObject::connect(&_timer, &QTimer::timeout, q, &Client::onTimerPingReq);
    QObject::connect(_network.data(), &Network::connected,
                     q, &Client::onNetworkConnected);
    QObject::connect(_network.data(), &Network::disconnected,
                     q, &Client::onNetworkDisconnected);
    QObject::connect(_network.data(), &Network::received,
                     q, &Client::onNetworkReceived);
    QObject::connect(_network.data(), &Network::error,
                     q, &Client::onNetworkError);
}

void QMQTT::ClientPrivate::initializeErrorHash()
{
    _socketErrorHash.insert(QAbstractSocket::ConnectionRefusedError, SocketConnectionRefusedError);
    _socketErrorHash.insert(QAbstractSocket::RemoteHostClosedError, SocketRemoteHostClosedError);
    _socketErrorHash.insert(QAbstractSocket::HostNotFoundError, SocketHostNotFoundError);
    _socketErrorHash.insert(QAbstractSocket::SocketAccessError, SocketAccessError);
    _socketErrorHash.insert(QAbstractSocket::SocketResourceError, SocketResourceError);
    _socketErrorHash.insert(QAbstractSocket::SocketTimeoutError, SocketTimeoutError);
    _socketErrorHash.insert(QAbstractSocket::DatagramTooLargeError, SocketDatagramTooLargeError);
    _socketErrorHash.insert(QAbstractSocket::NetworkError, SocketNetworkError);
    _socketErrorHash.insert(QAbstractSocket::AddressInUseError, SocketAddressInUseError);
    _socketErrorHash.insert(QAbstractSocket::SocketAddressNotAvailableError, SocketAddressNotAvailableError);
    _socketErrorHash.insert(QAbstractSocket::UnsupportedSocketOperationError, SocketUnsupportedSocketOperationError);
    _socketErrorHash.insert(QAbstractSocket::UnfinishedSocketOperationError, SocketUnfinishedSocketOperationError);
    _socketErrorHash.insert(QAbstractSocket::ProxyAuthenticationRequiredError, SocketProxyAuthenticationRequiredError);
    _socketErrorHash.insert(QAbstractSocket::SslHandshakeFailedError, SocketSslHandshakeFailedError);
    _socketErrorHash.insert(QAbstractSocket::ProxyConnectionRefusedError, SocketProxyConnectionRefusedError);
    _socketErrorHash.insert(QAbstractSocket::ProxyConnectionClosedError, SocketProxyConnectionClosedError);
    _socketErrorHash.insert(QAbstractSocket::ProxyConnectionTimeoutError, SocketProxyConnectionTimeoutError);
    _socketErrorHash.insert(QAbstractSocket::ProxyNotFoundError, SocketProxyNotFoundError);
    _socketErrorHash.insert(QAbstractSocket::ProxyProtocolError, SocketProxyProtocolError);
    _socketErrorHash.insert(QAbstractSocket::OperationError, SocketOperationError);
    _socketErrorHash.insert(QAbstractSocket::SslInternalError, SocketSslInternalError);
    _socketErrorHash.insert(QAbstractSocket::SslInvalidUserDataError, SocketSslInvalidUserDataError);
    _socketErrorHash.insert(QAbstractSocket::TemporaryError, SocketTemporaryError);
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
    ConnectPacket packet;

//    QString magic(PROTOCOL_MAGIC);
//    quint8 header = CONNECT;
//    quint8 flags = 0;

    packet.setCleanSession(_cleanSession);
    packet.setWillTopic(_willTopic);
    // todo: _willQos should be QosType
    packet.setWillQos(static_cast<QosType>(_willQos));
    packet.setWillRetain(_willRetain);
    packet.setUserName(_username);
    packet.setPassword(_password);
    packet.setKeepAlive(_keepAlive);
    packet.setClientIdentifier(_clientId);
    if(_clientId.isEmpty())
    {
        _clientId = randomClientId();
    }
    packet.setClientIdentifier(_clientId);

    //header

    // todo: why does this have a qos setting?
//    Frame frame(SETQOS(header, QOS1));
//    //flags
//    flags = FLAG_CLEANSESS(flags, _cleanSession ? 1 : 0 );
//    flags = FLAG_WILL(flags, willTopic().isEmpty() ? 0 : 1);
//    if (!willTopic().isEmpty())
//    {
//        flags = FLAG_WILLQOS(flags, willQos());
//        flags = FLAG_WILLRETAIN(flags, willRetain() ? 1 : 0);
//    }
//    if (!username().isEmpty())
//    {
//        flags = FLAG_USERNAME(flags, 1);
//    }
//    if (!password().isEmpty())
//    {
//        flags = FLAG_PASSWD(flags, 1);
//    }

//    //payload
//    frame.writeString(magic);
//    frame.writeChar(PROTOCOL_VERSION_MAJOR);
//    frame.writeChar(flags);
//    frame.writeInt(_keepAlive);
//    if(_clientId.isEmpty()) {
//        _clientId = randomClientId();
//    }
//    frame.writeString(_clientId);
//    if(!willTopic().isEmpty())
//    {
//        frame.writeString(willTopic());
//        if(!willMessage().isEmpty())
//        {
//            frame.writeString(willMessage());
//        }
//    }
//    if (!_username.isEmpty())
//    {
//        frame.writeString(_username);
//    }
//    if (!_password.isEmpty()){
//        frame.writeString(_password);
//    }
    _network->sendFrame(packet.toFrame());
}

quint16 QMQTT::ClientPrivate::sendPublish(const Message& message)
{
    PublishPacket packet;
    packet.setRetainFlag(message.retain());
    // todo: QMQTT::Message to use QosType
    packet.setQos(static_cast<QosType>(message.qos()));
    packet.setDupFlag(message.dup());
    packet.setTopicName(message.topic());
    packet.setPacketIdentifier(message.id());
    if (packet.qos() > Qos0)
    {
        if (packet.packetIdentifier() == 0)
        {
            packet.setPacketIdentifier(_gmid++);
        }
    }

    _network->sendFrame(packet.toFrame());
    return packet.packetIdentifier();
}

void QMQTT::ClientPrivate::sendPuback(quint8 /*type*/, quint16 /*mid*/)
{
//    Frame frame(type);
    Frame frame;
//    frame.writeInt(mid);
    _network->sendFrame(frame);
}

quint16 QMQTT::ClientPrivate::sendSubscribe(const QString& topic, const QosType qos)
{
    // todo: change this to support multiple subs
    quint16 mid = nextmid();

    SubscribePacket packet;
    packet.setPacketIdentifier(mid);
    Subscription subscription;
    subscription._topicFilter = topic;
    subscription._requestedQos = qos;
    packet.subscriptionList() << subscription;

    _network->sendFrame(packet.toFrame());
    return mid;
}

quint16 QMQTT::ClientPrivate::sendUnsubscribe(const QString& topic)
{
    // todo: support multiple topics unsubs
    quint16 mid = _gmid++;

    // todo: why is this old code trying to set a qos in the unsubframe?
    //    Frame frame(SETQOS(UNSUBSCRIBE, QOS1));

    QMQTT::UnsubscribePacket packet;
    packet.setPacketIdentifier(mid);
    packet.topicFilterList() << topic;
    _network->sendFrame(packet.toFrame());
    return mid;
}

void QMQTT::ClientPrivate::onTimerPingReq()
{
//    Frame frame(PINGREQ);
    Frame frame;
    _network->sendFrame(frame);
}

void QMQTT::ClientPrivate::disconnectFromHost()
{
    sendDisconnect();
    _network->disconnectFromHost();
}

void QMQTT::ClientPrivate::sendDisconnect()
{
    _network->sendFrame(DisconnectPacket().toFrame());
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
    // todo: change Client public interface to use QosType
    quint16 msgid = sendSubscribe(topic, static_cast<QosType>(qos));
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

void QMQTT::ClientPrivate::onNetworkReceived(const Frame& frame)
{
//    QMQTT::Frame frame(frm);
//    quint8 qos = 0;
//    bool retain, dup;
    QString topic;
//    quint16 mid = 0;
//    quint8 header = frame._header;
//    quint8 type = CONNACK;
    quint8 type = frame._header >> 4;
    Message message;

    switch (type)
    {
    case ConnackType:
//    case CONNACK:
//        frame.readChar();
        handleConnack(frame);
        break;
    case PublishType:
//        qos = GETQOS(header);;
//        retain = GETRETAIN(header);
//        dup = GETDUP(header);
//        topic = frame.readString();
//        if( qos > QOS0) {
//            mid = frame.readInt();
//        }
//        message.setId(mid);
//        message.setTopic(topic);
//        message.setPayload(frame.data());
//        message.setQos(qos);
//        message.setRetain(retain);
//        message.setDup(dup);
        handlePublish(frame);
        break;
//    case PUBACK:
//    case PUBREC:
//    case PUBREL:
//    case PUBCOMP:
//        mid = frame.readInt();
//        handlePuback(type, mid);
//        break;
//    case SUBACK:
//        mid = frame.readInt();
//        qos = frame.readChar();
        // todo: send a subscribed signal (only in certain cases? mid? qos?)
//        break;
//    case UNSUBACK:
        // todo: send an unsubscribed signal (only certain cases? mid?)
//        break;
//    case PINGRESP:
        // todo: I know I'm suppose to do something with this. Look at specifications.
//        break;
    default:
        break;
    }
}

void QMQTT::ClientPrivate::handleConnack(const Frame& frame)
{
    Q_UNUSED(frame);

    // todo: send connected signal
}

void QMQTT::ClientPrivate::handlePublish(const Frame& frame)
{
    Q_Q(Client);

    PublishPacket packet = PublishPacket::fromFrame(frame);

    if(packet.qos() == Qos1)
    {
//        sendPuback(PUBACK, message.id());
    }
    else if(packet.qos() == Qos2)
    {
//        sendPuback(PUBREC, message.id());
    }


    emit q->received(Message(
        packet.packetIdentifier(),
        packet.topicName(),
        packet.payload(),
        packet.qos(),
        packet.retainFlag(),
        packet.dupFlag()));
}

void QMQTT::ClientPrivate::handlePuback(const quint8 /*type*/, const quint16 /*msgid*/)
{
//    if(type == PUBREC)
//    {
//        sendPuback(PUBREL, msgid);
//    }
//    else if (type == PUBREL)
//    {
//        sendPuback(PUBCOMP, msgid);
//    }
    // todo: emit published signal (type? msgid?)
}

bool QMQTT::ClientPrivate::autoReconnect() const
{
    return _network->autoReconnect();
}

void QMQTT::ClientPrivate::setAutoReconnect(const bool autoReconnect)
{
    _network->setAutoReconnect(autoReconnect);
}

bool QMQTT::ClientPrivate::autoReconnectInterval() const
{
    return _network->autoReconnectInterval();
}

void QMQTT::ClientPrivate::setAutoReconnectInterval(const int autoReconnectInterval)
{
    _network->setAutoReconnectInterval(autoReconnectInterval);
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

void QMQTT::ClientPrivate::onNetworkError(QAbstractSocket::SocketError socketError)
{
    Q_Q(Client);
    emit q->error(_socketErrorHash.value(socketError, UnknownError));
}
