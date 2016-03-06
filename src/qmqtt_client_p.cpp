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
#include "qmqtt_pubackpacket.h"
#include "qmqtt_pubrecpacket.h"
#include "qmqtt_pubrelpacket.h"
#include "qmqtt_pubcomppacket.h"
#include "qmqtt_disconnectpacket.h"
#include "qmqtt_unsubscribepacket.h"
#include "qmqtt_subscribepacket.h"
#include "qmqtt_unsubackpacket.h"
#include "qmqtt_pingreqpacket.h"
#include "qmqtt_network.h"
#include "qmqtt_timer.h"
#include <QLoggingCategory>
#include <QDateTime>
#include <QUuid>

Q_LOGGING_CATEGORY(client, "qmqtt.client")

namespace
{
    const quint16 DEFAULT_KEEP_ALIVE_INTERVAL = 300;
}

static const quint8 QOS0 = 0x00;
static const quint8 QOS1 = 0x01;
static const quint8 QOS2 = 0x02;

QMQTT::ClientPrivate::ClientPrivate(Client* qq_ptr)
    : _host(QHostAddress::LocalHost)
    , _port(1883)
    , _gmid(1)
    , _clientId(QUuid::createUuid().toString())
    , _cleanSession(false)
    , _connectionState(InitializedState)
    , _willQos(0)
    , _willRetain(false)
    , q_ptr(qq_ptr)
{
}

QMQTT::ClientPrivate::~ClientPrivate()
{
}

void QMQTT::ClientPrivate::init(const QHostAddress& host, const quint16 port,
                                NetworkInterface* network,
                                TimerInterface* pingRespTimer,
                                TimerInterface* keepAliveTimer)
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

    if(keepAliveTimer == NULL)
    {
        _keepAliveTimer.reset(new Timer);
    }
    else
    {
        _keepAliveTimer.reset(keepAliveTimer);
    }
    _keepAliveTimer->setSingleShot(false);

    if(pingRespTimer == NULL)
    {
        _pingrespTimer.reset(new Timer);
    }
    else
    {
        _pingrespTimer.reset(pingRespTimer);
    }
    _pingrespTimer->setSingleShot(true);

    setKeepAlive(DEFAULT_KEEP_ALIVE_INTERVAL);

    initializeErrorHash();

    QObject::connect(_keepAliveTimer.data(), &Timer::timeout,
                     q, &Client::sendPingreqPacket);
    QObject::connect(_pingrespTimer.data(), &Timer::timeout,
                     _network.data(), &NetworkInterface::disconnectFromHost);

    QObject::connect(_network.data(), &NetworkInterface::connected,
                     q, &Client::onNetworkConnected);
    QObject::connect(_network.data(), &NetworkInterface::disconnected,
                     q, &Client::onNetworkDisconnected);
    QObject::connect(_network.data(), &NetworkInterface::received,
                     q, &Client::onNetworkReceived);
    QObject::connect(_network.data(), &NetworkInterface::error,
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
    setConnectionState(ConnectingState);
    _network->connectToHost(_host, _port);
}

void QMQTT::ClientPrivate::onNetworkConnected()
{
    ConnectPacket connectPacket;
    connectPacket.setCleanSession(_cleanSession);
    connectPacket.setWillTopic(_willTopic);
    // todo: _willQos should be QosType
    connectPacket.setWillQos(static_cast<QosType>(_willQos));
    connectPacket.setWillRetain(_willRetain);
    connectPacket.setUserName(_username);
    connectPacket.setPassword(_password);
    connectPacket.setKeepAlive(keepAlive());
    connectPacket.setClientIdentifier(_clientId);
    if(_clientId.isEmpty())
    {
        _clientId = randomClientId();
    }
    connectPacket.setClientIdentifier(_clientId);
    sendPacket(connectPacket);
}

void QMQTT::ClientPrivate::sendPingreqPacket()
{
    sendPacket(PingreqPacket());

    _pingrespTimer->start();
}

void QMQTT::ClientPrivate::disconnectFromHost()
{
    _network->sendFrame(DisconnectPacket().toFrame());

    _network->disconnectFromHost();
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

    PublishPacket publishPacket;
    publishPacket.setRetainFlag(message.retain());
    // todo: QMQTT::Message to use QosType
    publishPacket.setQos(static_cast<QosType>(message.qos()));
    publishPacket.setDupFlag(message.dup());
    publishPacket.setTopicName(message.topic());
    publishPacket.setPacketIdentifier(message.id());
    if (publishPacket.qos() > Qos0)
    {
        if (publishPacket.packetIdentifier() == 0)
        {
            publishPacket.setPacketIdentifier(_gmid++);
        }
    }

    sendPacket(publishPacket);

    emit q->published(message);

    return publishPacket.packetIdentifier();
}

quint16 QMQTT::ClientPrivate::subscribe(const QString& topic, const quint8 qos)
{
    // todo: change this to support multiple subs
    quint16 mid = nextmid();

    SubscribePacket subscribePacket;
    subscribePacket.setPacketIdentifier(mid);
    Subscription subscription;
    subscription._topicFilter = topic;
    // todo: change Client public interface to use QosType
    subscription._requestedQos = static_cast<QosType>(qos);
    subscribePacket.subscriptionList() << subscription;

    sendPacket(subscribePacket);

    return mid;
}

void QMQTT::ClientPrivate::unsubscribe(const QString& topic)
{
    // todo: support multiple topics unsubs
    quint16 mid = _gmid++;

    UnsubscribePacket unsubscribePacket;
    unsubscribePacket.setPacketIdentifier(mid);
    unsubscribePacket.topicFilterList() << topic;
    sendPacket(unsubscribePacket);
}

void QMQTT::ClientPrivate::onNetworkDisconnected()
{
    Q_Q(Client);

    setConnectionState(DisconnectedState);
    emit q->disconnected();
}

void QMQTT::ClientPrivate::onNetworkReceived(const Frame& frame)
{
    quint8 type = frame._header >> 4;

    switch (type)
    {
    case ConnectType:
        break;
    case ConnackType:
        connackPacketReceived(frame);
        break;
    case PublishType:
        publishPacketReceived(frame);
        break;
    case PubackType:
        pubackPacketReceived(frame);
        break;
    case PubrecType:
        pubrecPacketReceived(frame);
        break;
    case PubrelType:
        pubrelPacketReceived(frame);
        break;
    case PubcompType:
        pubcompPacketReceived(frame);
        break;
    case SubscribeType:
        break;
    case SubackType:
        subackPacketReceived(frame);
        break;
    case UnsubscribeType:
        break;
    case UnsubackType:
        unsubackPacketReceived(frame);
        break;
    case PingreqType:
        break;
    case PingrespType:
        pingrespPacketReceived(frame);
        break;
    case DisconnectType:
        disconnectPacketReceived(frame);
        break;
    default:
        break;
    }
}

void QMQTT::ClientPrivate::connackPacketReceived(const Frame& frame)
{
    Q_Q(Client);
    Q_UNUSED(frame);

    setConnectionState(ConnectedState);

    emit q->connected();
}

void QMQTT::ClientPrivate::publishPacketReceived(const Frame& frame)
{
    Q_Q(Client);

    PublishPacket packet = PublishPacket::fromFrame(frame);

    switch(packet.qos())
    {
    case Qos0:
    {
        Message message(
            packet.packetIdentifier(),
            packet.topicName(),
            packet.payload(),
            packet.qos(),
            packet.retainFlag(),
            packet.dupFlag());

        emit q->received(message);
    }
        break;
    case Qos1:
        break;
    case Qos2:
        break;
    default:
        break;
    };
}

void QMQTT::ClientPrivate::pubackPacketReceived(const Frame& frame)
{
    PubackPacket puback = PubackPacket::fromFrame(frame);
    Q_UNUSED(puback);
}

void QMQTT::ClientPrivate::pubrecPacketReceived(const Frame& frame)
{
    PubrecPacket pubrec = PubrecPacket::fromFrame(frame);

    // if this subscription is Qos2, then
    PubrelPacket pubrel;
    pubrel.setPacketIdentifier(pubrec.packetIdentifier());
    sendPacket(pubrel);
}

void QMQTT::ClientPrivate::pubrelPacketReceived(const Frame& frame)
{
    PubrelPacket pubrel = PubrelPacket::fromFrame(frame);

    // if this subscription is Qos2, then
    PubcompPacket pubcomp;
    pubcomp.setPacketIdentifier(pubrel.packetIdentifier());
    sendPacket(pubcomp);
}

void QMQTT::ClientPrivate::pubcompPacketReceived(const Frame& frame)
{
    PubcompPacket pubcomp = PubcompPacket::fromFrame(frame);
    Q_UNUSED(pubcomp);
}

void QMQTT::ClientPrivate::subackPacketReceived(const Frame& frame)
{
    Q_Q(Client);
    Q_UNUSED(frame);

    // todo: associate msgid with a tracked subscription to get topic
    // will need subscriptions and session support
    emit q->subscribed("");
}

void QMQTT::ClientPrivate::unsubackPacketReceived(const Frame& frame)
{
    Q_Q(Client);
    Q_UNUSED(frame);

    // todo: associate msgid with a tracked subscription to get topic
    // will need subscriptions and session support
    emit q->unsubscribed("");
}

void QMQTT::ClientPrivate::pingrespPacketReceived(const Frame& frame)
{
    Q_UNUSED(frame);

    _pingrespTimer->stop();
}

void QMQTT::ClientPrivate::disconnectPacketReceived(const Frame& frame)
{
    Q_UNUSED(frame);

    setConnectionState(DisconnectedState);
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
    _keepAliveTimer->setInterval(keepAlive * 1000);
    _pingrespTimer->setInterval(keepAlive * 1000);
}

int QMQTT::ClientPrivate::keepAlive() const
{
    return _keepAliveTimer->interval() / 1000;
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

void QMQTT::ClientPrivate::sendPacket(const AbstractPacket& packet)
{
    _network->sendFrame(packet.toFrame());
}

void QMQTT::ClientPrivate::setConnectionState(const ConnectionState& connectionState)
{
    if (_connectionState != connectionState)
    {
        if(connectionState == DisconnectedState)
        {
            _keepAliveTimer->stop();
            _pingrespTimer->stop();
        }
        else
        if(connectionState == ConnectedState)
        {
            _keepAliveTimer->start();
        }

        _connectionState = connectionState;
    }
}
