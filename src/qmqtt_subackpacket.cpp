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
#include <QDataStream>

const quint8 DEFAULT_FIXED_HEADER = QMQTT::SubackType << 4;

QMQTT::SubackPacket::SubackPacket()
    : AbstractPacket(DEFAULT_FIXED_HEADER)
    , _packetIdentifier(0)
    , _returnCode(SubackSuccessMaximumQoS0)
{
}

QMQTT::SubackPacket::~SubackPacket()
{
}

QMQTT::PacketType QMQTT::SubackPacket::type() const
{
    return QMQTT::SubackType;
}

qint64 QMQTT::SubackPacket::calculateRemainingLengthFromData() const
{
    return 2;
}

bool QMQTT::SubackPacket::isValid() const
{
    if (_fixedHeader != DEFAULT_FIXED_HEADER)
    {
        return false;
    }

    if (_returnCode > 0x02 && _returnCode != 0x80)
    {
        return false;
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

QMQTT::SubackReturnCodeType QMQTT::SubackPacket::returnCode() const
{
    return _returnCode;
}

void QMQTT::SubackPacket::setReturnCode(const SubackReturnCodeType returnCode)
{
    _returnCode = returnCode;
}

QDataStream& QMQTT::operator>>(QDataStream& stream, SubackPacket& packet)
{
    stream >> packet._fixedHeader;
    packet.readRemainingLength(stream);
    stream >> packet._packetIdentifier;
    quint8 i = 0;
    stream >> i;
    packet._returnCode = static_cast<SubackReturnCodeType>(i);

    return stream;
}

QDataStream& QMQTT::operator<<(QDataStream& stream, const SubackPacket& packet)
{
    stream << packet._fixedHeader;
    packet.writeRemainingLength(stream);
    stream << packet._packetIdentifier;
    stream << static_cast<quint8>(packet._returnCode);

    return stream;
}
