/*
 * qmqtt_client.cpp - qmqtt client
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

#include "qmqtt_client.h"
#include "qmqtt_client_p.h"

namespace QMQTT {

Client::Client(const QString & host, quint32 port, QObject * parent /* =0 */)
:d_ptr(new ClientPrivate(this))

{
    Q_D(Client);
    d->init(host, port, parent);
}

Client::~Client()
{
    //?
    Q_D(Client);
    delete d;
}

/*----------------------------------------------------------------
 * Get/Set Property
 ----------------------------------------------------------------*/
QString Client::host() const
{
    Q_D(const Client);
//     QString* str = d->host;
//    return const_cast<QString *const>(d->host);
//    return d->host;
    return d->host;
}

void Client::setHost(const QString & host)
{
//    d->host = host;
    Q_D(Client);
    d->host = host;
}

quint32 Client::port() const
{
    Q_D(const Client);
    return d->port;
}

void Client::setPort(quint32 port)
{
    Q_D(Client);
    d->port = port;
}

QString Client::clientId() const
{
     Q_D(const Client);
    return d->clientId;
}

void Client::setClientId(const QString &clientId)
{
     Q_D(Client);
    d->clientId = clientId;
}

QString Client::username() const
{
     Q_D(const Client);
    return d->username;
}

void Client::setUsername(const QString & username)
{
     Q_D(Client);
    d->username = username;
}

QString Client::password() const
{
     Q_D(const Client);
    return d->password;
}

void Client::setPassword(const QString & password)
{
     Q_D(Client);
    d->password = password;
}

int Client::keepalive()
{
     Q_D( Client);
    return d->keepalive;
}

void Client::setKeepAlive(int keepalive)
{
     Q_D(Client);
    d->keepalive = keepalive;
}

bool Client::cleansess()
{
     Q_D(Client);
    return d->cleansess;
}

void Client::setCleansess(bool cleansess)
{
     Q_D(Client);
    d->cleansess = cleansess;
}

bool Client::autoReconnect() const
{
    Q_D(const Client);
    return d->network->autoReconnect();
}

void Client::setAutoReconnect(bool value)
{
    Q_D(Client);
    d->network->setAutoReconnect(value);
}

Will *Client::will()
{
    Q_D(Client);
    return d->will;
}

void Client::setWill(Will *will)
{
    Q_D(Client);
    d->will = will;
}

State Client::state() const
{
    Q_D(const Client);
    return d->state;
}

bool Client::isConnected()
{
    Q_D(Client);
    return d->network->isConnected();
}


/*----------------------------------------------------------------
 * MQTT Command
 ----------------------------------------------------------------*/
void Client::connect()
{
    Q_D(Client);
    d->sockConnect();
}

void Client::onConnected()
{
    Q_D(Client);
    qCDebug(client) << "Sock Connected....";
    d->sendConnect();
    d->startKeepalive();
    emit connected();
}

quint16 Client::publish(Message &message)
{
    Q_D(Client);
    quint16 msgid = d->sendPublish(message);
    emit published(message);
    return msgid;
}

void Client::puback(quint8 type, quint16 msgid)
{
    Q_D(Client);
    d->sendPuback(type, msgid);
    emit pubacked(type, msgid);
}

quint16 Client::subscribe(const QString &topic, quint8 qos)
{
    Q_D(Client);
    quint16 msgid = d->sendSubscribe(topic, qos);
    emit subscribed(topic);
    return msgid;
}

void Client::unsubscribe(const QString &topic)
{
   Q_D(Client);
    d->sendUnsubscribe(topic);
    emit unsubscribed(topic);
}

void Client::ping()
{
    Q_D(Client);
    d->sendPing();
}

void Client::disconnect()
{
    Q_D(Client);
    d->disconnect();
}

void Client::onDisconnected()
{
    Q_D(Client);
    d->stopKeepalive();
    emit disconnected();
}

//---------------------------------------------
//---------------------------------------------
void Client::onReceived(Frame &frame)
{
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
        if( qos > MQTT_QOS0) {
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
        emit subacked(mid, qos);
        break;
    case UNSUBACK:
        emit unsubacked(mid);
        break;
    case PINGRESP:
        emit pong();
        break;
    default:
        break;
    }
}

void Client::handleConnack(quint8 ack)
{
    qCDebug(client) << "connack: " << ack;
    emit connacked(ack);
}

void Client::handlePublish(Message & message)
{
    Q_D(Client);
    if(message.qos() == MQTT_QOS1) {
        d->sendPuback(PUBACK, message.id());
    } else if(message.qos() == MQTT_QOS2) {
        d->sendPuback(PUBREC, message.id());
    }
    emit received(message);
}

void Client::handlePuback(quint8 type, quint16 msgid)
{
    Q_D(Client);
    if(type == PUBREC) {
        d->sendPuback(PUBREL, msgid);
    } else if (type == PUBREL) {
        d->sendPuback(PUBCOMP, msgid);
    }
    emit pubacked(type, msgid);
}

} // namespace QMQTT

