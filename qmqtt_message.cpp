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

namespace QMQTT {

Message::Message() :
    _id(0),
    _qos(0),
    _retain(false),
    _dup(false)
{
}

Message::Message(quint16 id, const QString &topic, const QByteArray &payload,
            quint8 qos, bool retain, bool dup) :
    _id(id),
    _topic(topic),
    _payload(payload),
    _qos(qos),
    _retain(retain),
    _dup(dup)
{

}

Message::~Message()
{
    //NOTHING TODO
}

quint16 Message::id()
{
    return _id;
}

void Message::setId(quint16 id)
{
    _id = id;
}

quint8 Message::qos() const
{
    return _qos;
}

void Message::setQos(quint8 qos)
{
    _qos = qos;
}

bool Message::retain()
{
    return _retain;
}

void Message::setRetain(bool retain)
{
    _retain = retain;
}

bool Message::dup()
{
    return _dup;
}
void Message::setDup(bool dup)
{
    _dup =dup;
}

QString Message::topic() const
{
    return _topic;
}

void Message::setTopic(const QString & topic)
{
    _topic = topic;
}

QByteArray Message::payload() const
{
    return _payload;
}

void Message::setPayload(const QByteArray &payload)
{
    _payload = payload;
}

} // namespace QMQTT
