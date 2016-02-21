/*
 * qmqtt_connackpacket.cpp - qmqtt connack packet
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
#include "qmqtt_connackpacket.h"

const quint8 DEFAULT_FIXED_HEADER = QMQTT::ConnackType << 4;
const quint8 NO_MASK = 0x00;
const quint8 RESERVED_MASK = 0xfe;
const quint8 SESSION_PRESENT_MASK = 0x01;

QMQTT::ConnackPacket::ConnackPacket()
    : AbstractPacket(DEFAULT_FIXED_HEADER)
    , _connectAcknowledgeFlags(NO_MASK)
    , _connectReturnCode(ConnectionAccepted)
{
}

QMQTT::ConnackPacket::~ConnackPacket()
{
}

QMQTT::PacketType QMQTT::ConnackPacket::type() const
{
    return QMQTT::ConnackType;
}

qint64 QMQTT::ConnackPacket::calculateRemainingLengthFromData() const
{
    return 2;
}

bool QMQTT::ConnackPacket::sessionPresent() const
{
    return (_connectAcknowledgeFlags & SESSION_PRESENT_MASK) != 0;
}

void QMQTT::ConnackPacket::setSessionPresent(const bool sessionPresent)
{
    _connectAcknowledgeFlags &= ~SESSION_PRESENT_MASK;
    if (sessionPresent)
    {
        _connectAcknowledgeFlags |= SESSION_PRESENT_MASK;
    }
}

QMQTT::ConnackPacket::ConnectReturnCodeType QMQTT::ConnackPacket::connectReturnCode() const
{
    return _connectReturnCode;
}

void QMQTT::ConnackPacket::setConnectReturnCode(const ConnectReturnCodeType connectReturnCode)
{
    _connectReturnCode = connectReturnCode;
}

bool QMQTT::ConnackPacket::isValid() const
{
    if (_fixedHeader != DEFAULT_FIXED_HEADER)
    {
        return false;
    }

    if ((_connectAcknowledgeFlags & RESERVED_MASK) != 0)
    {
        return false;
    }

    if (!sessionPresent())
    {
        if (connectReturnCode() != ConnectionAccepted)
        {
            return false;
        }
    }

    return true;
}

QDataStream& QMQTT::operator>>(QDataStream& stream, ConnackPacket& packet)
{
    stream >> packet._fixedHeader;
    packet.readRemainingLength(stream);
    stream >> packet._connectAcknowledgeFlags;
    quint8 i = 0;
    stream >> i;
    packet._connectReturnCode = static_cast<QMQTT::ConnackPacket::ConnectReturnCodeType>(i);

    return stream;
}

QDataStream& QMQTT::operator<<(QDataStream& stream, const ConnackPacket& packet)
{
    stream << packet._fixedHeader;
    packet.writeRemainingLength(stream);
    stream << packet._connectAcknowledgeFlags;
    stream << static_cast<quint8>(packet._connectReturnCode);

    return stream;
}
