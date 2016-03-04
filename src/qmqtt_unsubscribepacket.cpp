/*
 * qmqtt_unsubscribepacket.cpp - qmqtt unsubscribe packet
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
#include "qmqtt_unsubscribepacket.h"
#include <QBuffer>

const quint8 DEFAULT_FIXED_HEADER = (QMQTT::UnsubscribeType << 4) | 0x02;

QMQTT::UnsubscribePacket::UnsubscribePacket()
    : _packetIdentifier(0)
    , _headerReservedBitsValid(true)
{
}

QMQTT::UnsubscribePacket::~UnsubscribePacket()
{
}

QMQTT::PacketType QMQTT::UnsubscribePacket::type() const
{
    return QMQTT::UnsubscribeType;
}

bool QMQTT::UnsubscribePacket::isValid() const
{
    if (!_headerReservedBitsValid)
    {
        return false;
    }

    if (_topicFilterList.isEmpty())
    {
        return false;
    }

    foreach (const QString& topicFilter, _topicFilterList)
    {
        if (topicFilter.isEmpty())
        {
            return false;
        }
    }

    return true;
}

quint16 QMQTT::UnsubscribePacket::packetIdentifier() const
{
    return _packetIdentifier;
}

void QMQTT::UnsubscribePacket::setPacketIdentifier(const quint16 packetIdentifier)
{
    _packetIdentifier = packetIdentifier;
}

QStringList& QMQTT::UnsubscribePacket::topicFilterList()
{
    return _topicFilterList;
}

QMQTT::Frame QMQTT::UnsubscribePacket::toFrame() const
{
    Frame frame;

    frame._header = static_cast<quint8>(type()) << 4;

    QBuffer buffer(&frame._data);
    buffer.open(QIODevice::WriteOnly);
    QDataStream stream(&buffer);
    stream << _packetIdentifier;
    foreach (const QString& topicFilter, _topicFilterList)
    {
        writeString(stream, topicFilter);
    }
    buffer.close();

    return frame;
}

QMQTT::UnsubscribePacket QMQTT::UnsubscribePacket::fromFrame(Frame& frame)
{
    UnsubscribePacket packet;

    packet._headerReservedBitsValid = (frame._header & 0x0f) == 0;

    QBuffer buffer(&frame._data);
    buffer.open(QIODevice::ReadOnly);
    QDataStream stream(&buffer);
    stream >> packet._packetIdentifier;
    while (buffer.pos() < buffer.size())
    {
        packet._topicFilterList << readString(stream);
    }
    buffer.close();

    return packet;
}
