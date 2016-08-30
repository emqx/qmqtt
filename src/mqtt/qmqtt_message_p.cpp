/*
 * qmqtt_message.cpp - qmqtt message private
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
#include "qmqtt_message_p.h"

QMQTT::MessagePrivate::MessagePrivate()
    : _id(0)
    , _qos(0)
    , _retain(false)
    , _dup(false)
{
}

QMQTT::MessagePrivate::~MessagePrivate()
{
}

void QMQTT::MessagePrivate::init(const quint16 id, const QString& topic,
                                 const QByteArray& payload, const quint8 qos,
                                 const bool retain, const bool dup)
{
    _id = id;
    _topic = topic;
    _payload = payload;
    _qos = qos;
    _retain = retain;
    _dup = dup;
}

QMQTT::MessagePrivate::MessagePrivate(const MessagePrivate& other)
{
    _id = other._id;
    _topic = other._topic;
    _payload = other._payload;
    _qos = other._qos;
    _retain = other._retain;
    _dup = other._dup;
}

QMQTT::MessagePrivate& QMQTT::MessagePrivate::operator=(const MessagePrivate& other)
{
    if(this != &other)
    {
        _id = other._id;
        _topic = other._topic;
        _payload = other._payload;
        _qos = other._qos;
        _retain = other._retain;
        _dup = other._dup;
    }
    return *this;
}

bool QMQTT::MessagePrivate::operator==(const MessagePrivate& other) const
{
  return _id == other._id
      && _topic == other._topic
      && _payload == other._payload
      && _qos == other._qos
      && _retain == other._retain
      && _dup == other._dup;
}

quint16 QMQTT::MessagePrivate::id() const
{
    return _id;
}

void QMQTT::MessagePrivate::setId(const quint16 id)
{
    _id = id;
}

quint8 QMQTT::MessagePrivate::qos() const
{
    return _qos;
}

void QMQTT::MessagePrivate::setQos(const quint8 qos)
{
    _qos = qos;
}

bool QMQTT::MessagePrivate::retain() const
{
    return _retain;
}

void QMQTT::MessagePrivate::setRetain(const bool retain)
{
    _retain = retain;
}

bool QMQTT::MessagePrivate::dup() const
{
    return _dup;
}
void QMQTT::MessagePrivate::setDup(const bool dup)
{
    _dup =dup;
}

QString QMQTT::MessagePrivate::topic() const
{
    return _topic;
}

void QMQTT::MessagePrivate::setTopic(const QString& topic)
{
    _topic = topic;
}

QByteArray QMQTT::MessagePrivate::payload() const
{
    return _payload;
}

void QMQTT::MessagePrivate::setPayload(const QByteArray& payload)
{
    _payload = payload;
}
