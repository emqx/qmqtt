/*
 * qmqtt_subsribepacket.cpp - qmqtt subscribe packet
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
#include "qmqtt_subscribepacket.h"
#include <QBuffer>

QMQTT::SubscribePacket::SubscribePacket()
    : _packetIdentifier(0)
    , _headerReservedBitsValid(true)
{
}

QMQTT::SubscribePacket::~SubscribePacket()
{
}

QMQTT::PacketType QMQTT::SubscribePacket::type() const
{
    return QMQTT::SubscribeType;
}

bool QMQTT::SubscribePacket::isValid() const
{
    if (!_headerReservedBitsValid)
    {
        return false;
    }

    if (_subscriptionList.isEmpty())
    {
        return false;
    }

    foreach (const Subscription& subscription, _subscriptionList)
    {
        if (subscription._topicFilter.isEmpty())
        {
            return false;
        }

        if (static_cast<quint8>(subscription._requestedQos) > 2)
        {
            return false;
        }
    }

    return true;
}

quint16 QMQTT::SubscribePacket::packetIdentifier() const
{
    return _packetIdentifier;
}

void QMQTT::SubscribePacket::setPacketIdentifier(const quint16 packetIdentifier)
{
    _packetIdentifier = packetIdentifier;
}

QMQTT::SubscriptionList& QMQTT::SubscribePacket::subscriptionList()
{
    return _subscriptionList;
}

QMQTT::Frame QMQTT::SubscribePacket::toFrame() const
{
    Frame frame;

    frame._header = static_cast<quint8>(type()) << 4;

    QBuffer buffer(&frame._data);
    buffer.open(QIODevice::WriteOnly);
    QDataStream stream(&buffer);
    stream << _packetIdentifier;
    foreach (const Subscription& subscription, _subscriptionList)
    {
        writeString(stream, subscription._topicFilter);
        stream << static_cast<quint8>(subscription._requestedQos);
    }
    buffer.close();

    return frame;
}

QMQTT::SubscribePacket QMQTT::SubscribePacket::fromFrame(const Frame& frame)
{
    SubscribePacket packet;

    packet._headerReservedBitsValid = (frame._header & 0x0f) == 0;

    QBuffer buffer;
    buffer.setData(frame._data);
    buffer.open(QIODevice::ReadOnly);
    QDataStream stream(&buffer);
    stream >> packet._packetIdentifier;
    while (buffer.pos() < buffer.size())
    {
        Subscription sub;
        sub._topicFilter = readString(stream);
        quint8 i = 0;
        stream >> i;
        sub._requestedQos = static_cast<QosType>(i);
        packet._subscriptionList.append(sub);
    }
    buffer.close();

    return packet;
}
