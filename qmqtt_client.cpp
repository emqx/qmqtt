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

QMQTT::Client::Client(const QString host, const quint16 port, QObject* parent)
    : QObject(parent)
    , d_ptr(new ClientPrivate(this))
{
    Q_D(Client);
    d->init(host, port);
}

QMQTT::Client::~Client()
{
}

QString QMQTT::Client::host() const
{
    Q_D(const Client);
    return d->host();
}

void QMQTT::Client::setHost(const QString& host)
{
    Q_D(Client);
    d->setHost(host);
}

quint16 QMQTT::Client::port() const
{
    Q_D(const Client);
    return d->port();
}

void QMQTT::Client::setPort(const quint16 port)
{
    Q_D(Client);
    d->setPort(port);
}

QString QMQTT::Client::clientId() const
{
    Q_D(const Client);
    return d->clientId();
}

void QMQTT::Client::setClientId(const QString& clientId)
{
    Q_D(Client);
    d->setClientId(clientId);
}

QString QMQTT::Client::username() const
{
    Q_D(const Client);
    return d->username();
}

void QMQTT::Client::setUsername(const QString& username)
{
    Q_D(Client);
    d->setUsername(username);
}

QString QMQTT::Client::password() const
{
    Q_D(const Client);
    return d->password();
}

void QMQTT::Client::setPassword(const QString& password)
{
    Q_D(Client);
    d->setPassword(password);
}

int QMQTT::Client::keepalive() const
{
    Q_D(const Client);
    return d->keepalive();
}

void QMQTT::Client::setKeepAlive(const int keepalive)
{
    Q_D(Client);
    d->setKeepAlive(keepalive);
}

bool QMQTT::Client::cleansess()
{
    Q_D(Client);
    return d->cleansess();
}

void QMQTT::Client::setCleansess(const bool cleansess)
{
    Q_D(Client);
    d->setCleansess(cleansess);
}

bool QMQTT::Client::autoReconnect() const
{
    Q_D(const Client);
    return d->autoReconnect();
}

void QMQTT::Client::setAutoReconnect(const bool value)
{
    Q_D(Client);
    d->setAutoReconnect(value);
}

QMQTT::Will* QMQTT::Client::will() const
{
    Q_D(const Client);
    return d->will();
}

void QMQTT::Client::setWill(Will* will)
{
    Q_D(Client);
    d->setWill(will);
}

QMQTT::ClientState QMQTT::Client::state() const
{
    Q_D(const Client);
    return d->state();
}

bool QMQTT::Client::isConnectedToHost() const
{
    Q_D(const Client);
    return d->isConnectedToHost();
}

/*----------------------------------------------------------------
 * MQTT Command
 ----------------------------------------------------------------*/
void QMQTT::Client::connectToHost()
{
    Q_D(Client);
    d->connectToHost();
}

void QMQTT::Client::onNetworkConnected()
{    
    Q_D(Client);
    d->onNetworkConnected();
}

quint16 QMQTT::Client::publish(Message& message)
{
    Q_D(Client);
    return d->publish(message);
}

void QMQTT::Client::puback(const quint8 type, const quint16 msgid)
{
    Q_D(Client);
    d->puback(type, msgid);
}

quint16 QMQTT::Client::subscribe(const QString& topic, const quint8 qos)
{
    Q_D(Client);
    return d->subscribe(topic, qos);
}

void QMQTT::Client::unsubscribe(const QString& topic)
{
    Q_D(Client);
    d->unsubscribe(topic);
}

void QMQTT::Client::ping()
{
    Q_D(Client);
    d->ping();
}

void QMQTT::Client::disconnectFromHost()
{
    Q_D(Client);
    d->disconnectFromHost();
}

void QMQTT::Client::onNetworkDisconnected()
{
    Q_D(Client);
    d->onNetworkDisconnected();
}

void QMQTT::Client::onReceived(const QMQTT::Frame& frame)
{
    Q_D(Client);
    d->onReceived(frame);
}
