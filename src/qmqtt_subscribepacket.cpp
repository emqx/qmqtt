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
#include <QDataStream>

const quint8 DEFAULT_FIXED_HEADER = (QMQTT::SubscribeType << 4) | 0x02;

QMQTT::SubscribePacket::SubscribePacket()
    : AbstractPacket(DEFAULT_FIXED_HEADER)
    , _packetIdentifier(0)
{
}

QMQTT::SubscribePacket::~SubscribePacket()
{
}

QMQTT::PacketType QMQTT::SubscribePacket::type() const
{
    return QMQTT::SubscribeType;
}

qint64 QMQTT::SubscribePacket::calculateRemainingLengthFromData() const
{
    quint64 remainingLength = 2;
    foreach (const Subscription& subscription, _subscriptionList)
    {
        remainingLength += subscription._topicFilter.size() + 3;
    }
    return remainingLength;
}

bool QMQTT::SubscribePacket::isValid() const
{
    if (_fixedHeader != DEFAULT_FIXED_HEADER)
    {
        return false;
    }

    // todo: re-check all wildcards

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

        if (subscription._requestedQos > 2)
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

QMQTT::SubscriptionList QMQTT::SubscribePacket::subscriptionList() const
{
    return _subscriptionList;
}

void QMQTT::SubscribePacket::setSubscriptionList(const SubscriptionList subscriptionList)
{
    _subscriptionList = subscriptionList;
}

QDataStream& QMQTT::operator>>(QDataStream& stream, SubscribePacket& packet)
{
    stream >> packet._fixedHeader;
    qint64 remainingLength = packet.readRemainingLength(stream);
    stream >> packet._packetIdentifier;
    remainingLength -= 2;
    packet._subscriptionList.clear();
    while (remainingLength > 0)
    {
        Subscription subscription;
        subscription._topicFilter = packet.readString(stream);
        stream >> subscription._requestedQos;

        packet._subscriptionList.append(subscription);
        remainingLength -= subscription._topicFilter.size() + 3;
    }
    return stream;
}

QDataStream& QMQTT::operator<<(QDataStream& stream, const SubscribePacket& packet)
{
    stream << packet._fixedHeader;
    packet.writeRemainingLength(stream);
    stream << packet._packetIdentifier;
    foreach (const Subscription& subscription, packet._subscriptionList)
    {
        packet.writeString(stream, subscription._topicFilter);
        stream << subscription._requestedQos;
    }
    return stream;
}
