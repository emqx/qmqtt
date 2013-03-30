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
    :pd_ptr(new ClientPrivate(this))
{
    pd_func()->init(host, port, parent);
}

Client::~Client()
{
    //?
}

/*----------------------------------------------------------------
 * Get/Set Property
 ----------------------------------------------------------------*/
QString Client::host() const
{
    return pd_func()->host;
}

void Client::setHost(const QString & host)
{
    pd_func()->host = host;
}

quint32 Client::port() const
{
    return pd_func()->port;
}

void Client::setPort(quint32 port)
{
    pd_func()->port = port;
}

QString Client::clientId() const
{
    return pd_func()->clientId;
}

void Client::setClientId(const QString &clientId)
{
    pd_func()->clientId = clientId;
}

QString Client::username() const
{
    return pd_func()->username;
}

void Client::setUsername(const QString & username)
{
    pd_func()->username = username;
}

QString Client::password() const
{
    return pd_func()->password;
}

void Client::setPassword(const QString & password)
{
    pd_func()->password = password;
}

int Client::keepalive()
{
    return pd_func()->keepalive;
}

void Client::setKeepAlive(int keepalive)
{
    pd_func()->keepalive = keepalive;
}

bool Client::cleansess()
{
    return pd_func()->cleansess;
}

void Client::setCleansess(bool cleansess)
{
    pd_func()->cleansess = cleansess;
}

bool Client::autoReconnect() const
{
    return pd_func()->network->autoReconnect();
}

void Client::setAutoReconnect(bool value)
{
    pd_func()->network->setAutoReconnect(value);
}

Will *Client::will()
{
    return pd_func()->will;
}

void Client::setWill(Will *will)
{
    pd_func()->will = will;
}

State Client::state() const
{
    return pd_func()->state;
}

bool Client::isConnected()
{
    return pd_func()->network->isConnected();
}


/*----------------------------------------------------------------
 * MQTT Command
 ----------------------------------------------------------------*/
void Client::connect()
{
    pd_func()->sockConnect();
}

void Client::onConnected()
{
    qDebug("Sock Connected....");
    pd_func()->sendConnect();
    pd_func()->startKeepalive();
    emit connected();
}

quint16 Client::publish(Message &message)
{
    quint16 msgid = pd_func()->sendPublish(message);
    emit published(message);
    return msgid;
}

void Client::puback(quint8 type, quint16 msgid)
{
    pd_func()->sendPuback(type, msgid);
    emit pubacked(type, msgid);
}

quint16 Client::subscribe(const QString &topic, quint8 qos)
{
    quint16 msgid = pd_func()->sendSubscribe(topic, qos);
    emit subscribed(topic);
    return msgid;
}

void Client::unsubscribe(const QString &topic)
{
    pd_func()->sendUnsubscribe(topic);
    emit unsubscribed(topic);
}

void Client::ping()
{
    pd_func()->sendPing();
}

void Client::disconnect()
{
    pd_func()->disconnect();
}

void Client::onDisconnected()
{
    pd_func()->stopKeepalive();
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
    qDebug("handleFrame: type=%d", type);

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
    qDebug("connack: %d", ack);
    emit connacked(ack);
}

void Client::handlePublish(Message & message)
{
    if(message.qos() == MQTT_QOS1) {
        pd_func()->sendPuback(PUBACK, message.id());
    } else if(message.qos() == MQTT_QOS2) {
        pd_func()->sendPuback(PUBREC, message.id());
    }
    emit received(message);
}

void Client::handlePuback(quint8 type, quint16 msgid)
{
    if(type == PUBREC) {
        pd_func()->sendPuback(PUBREL, msgid);
    } else if (type == PUBREL) {
        pd_func()->sendPuback(PUBCOMP, msgid);
    }
    emit pubacked(type, msgid);
}

} // namespace QMQTT

