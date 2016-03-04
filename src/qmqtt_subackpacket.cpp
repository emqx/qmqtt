/*
 * qmqtt_subackpacket.cpp - qmqtt suback packet
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
#include "qmqtt_subackpacket.h"
#include <QBuffer>

QMQTT::SubackPacket::SubackPacket()
    : _packetIdentifier(0)
    , _headerReservedBitsValid(true)
{
}

QMQTT::SubackPacket::~SubackPacket()
{
}

QMQTT::PacketType QMQTT::SubackPacket::type() const
{
    return QMQTT::SubackType;
}

bool QMQTT::SubackPacket::isValid() const
{
    if (_returnCodeList.isEmpty())
    {
        return false;
    }

    foreach (const SubackReturnCodeType& returnCode, _returnCodeList)
    {
        if (static_cast<quint8>(returnCode > 0x02)
            && static_cast<quint8>(returnCode != 0x80))
        {
            return false;
        }
    }

    return true;
}

quint16 QMQTT::SubackPacket::packetIdentifier() const
{
    return _packetIdentifier;
}

void QMQTT::SubackPacket::setPacketIdentifier(const quint16 packetIdentifier)
{
    _packetIdentifier = packetIdentifier;
}

QMQTT::SubackReturnCodeList& QMQTT::SubackPacket::returnCodeList()
{
    return _returnCodeList;
}

QMQTT::Frame QMQTT::SubackPacket::toFrame() const
{
    Frame frame;

    frame._header = static_cast<quint8>(type()) << 4;

    QBuffer buffer(&frame._data);
    buffer.open(QIODevice::WriteOnly);
    QDataStream stream(&buffer);
    stream << _packetIdentifier;
    foreach (const SubackReturnCodeType& returnCode, _returnCodeList)
    {
        stream << static_cast<quint8>(returnCode);
    }
    buffer.close();

    return frame;
}

QMQTT::SubackPacket QMQTT::SubackPacket::fromFrame(Frame& frame)
{
    SubackPacket packet;

    packet._headerReservedBitsValid = (frame._header & 0x0f) == 0;

    QBuffer buffer(&frame._data);
    buffer.open(QIODevice::ReadOnly);
    QDataStream stream(&buffer);
    stream >> packet._packetIdentifier;
    while (buffer.pos() < buffer.size())
    {
        quint8 i = 0;
        stream >> i;
        packet._returnCodeList << static_cast<SubackReturnCodeType>(i);
    }
    buffer.close();

    return packet;
}
