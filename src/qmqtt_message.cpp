/*
 * qmqtt_message.cpp - qmqtt message
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
#include "qmqtt_message.h"
#include "qmqtt_message_p.h"

QMQTT::Message::Message()
    : d_ptr(new MessagePrivate)
{
    Q_D(Message);
    d->init(0, QString(), QByteArray(), 0, false, false);
}

QMQTT::Message::Message(quint16 id, const QString &topic, const QByteArray &payload,
                 quint8 qos, bool retain, bool dup)
    : d_ptr(new MessagePrivate)
{
    Q_D(Message);
    d->init(id, topic, payload, qos, retain, dup);
}

QMQTT::Message::~Message()
{
}

QMQTT::Message::Message(const Message& other)
    : d_ptr(new MessagePrivate(*(other.d_ptr)))
{
}

QMQTT::Message& QMQTT::Message::operator=(const Message& other)
{
    Q_D(Message);
    *d = *(other.d_ptr);
    return *this;
}

bool QMQTT::Message::operator==(const Message& other) const
{
    Q_D(const Message);
    return *d == *(other.d_ptr);
}

quint16 QMQTT::Message::id() const
{
    Q_D(const Message);
    return d->id();
}

void QMQTT::Message::setId(const quint16 id)
{
    Q_D(Message);
    d->setId(id);
}

quint8 QMQTT::Message::qos() const
{
    Q_D(const Message);
    return d->qos();
}

void QMQTT::Message::setQos(const quint8 qos)
{
    Q_D(Message);
    d->setQos(qos);
}

bool QMQTT::Message::retain() const
{
    Q_D(const Message);
    return d->retain();
}

void QMQTT::Message::setRetain(const bool retain)
{
    Q_D(Message);
    d->setRetain(retain);
}

bool QMQTT::Message::dup() const
{
    Q_D(const Message);
    return d->dup();
}

void QMQTT::Message::setDup(const bool dup)
{
    Q_D(Message);
    d->setDup(dup);
}

QString QMQTT::Message::topic() const
{
    Q_D(const Message);
    return d->topic();
}

void QMQTT::Message::setTopic(const QString& topic)
{
    Q_D(Message);
    d->setTopic(topic);
}

QByteArray QMQTT::Message::payload() const
{
    Q_D(const Message);
    return d->payload();
}

void QMQTT::Message::setPayload(const QByteArray& payload)
{
    Q_D(Message);
    d->setPayload(payload);
}
