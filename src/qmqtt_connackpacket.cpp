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
#include <QBuffer>

namespace
{
    const quint8 DEFAULT_HEADER = QMQTT::ConnackType << 4;
}

QMQTT::ConnackPacket::ConnackPacket()
    : _sessionPresent(false)
    , _connectReturnCode(ConnectionAccepted)
    , _headerReservedBitsValid(true)
    , _connectAcknowledgeBitsValid(true)
{
}

QMQTT::ConnackPacket::~ConnackPacket()
{
}

QMQTT::PacketType QMQTT::ConnackPacket::type() const
{
    return ConnackType;
}

bool QMQTT::ConnackPacket::sessionPresent() const
{
    return _sessionPresent;
}

void QMQTT::ConnackPacket::setSessionPresent(const bool sessionPresent)
{
    _sessionPresent = sessionPresent;
}

QMQTT::ConnectReturnCodeType QMQTT::ConnackPacket::connectReturnCode() const
{
    return _connectReturnCode;
}

void QMQTT::ConnackPacket::setConnectReturnCode(const ConnectReturnCodeType connectReturnCode)
{
    _connectReturnCode = connectReturnCode;
}

bool QMQTT::ConnackPacket::isValid() const
{
    if (!_headerReservedBitsValid)
    {
        return false;
    }

    if (!_connectAcknowledgeBitsValid)
    {
        return false;
    }

    if (!_sessionPresent)
    {
        if (_connectReturnCode != ConnectionAccepted)
        {
            return false;
        }
    }

    return true;
}

QMQTT::ConnackPacket QMQTT::ConnackPacket::fromFrame(const Frame& frame)
{
    ConnackPacket packet;

    packet._headerReservedBitsValid = (frame._header & 0x0f) == 0;

    QBuffer buffer;
    buffer.setData(frame._data);
    buffer.open(QIODevice::ReadOnly);
    QDataStream stream(&buffer);

    quint8 i = 0;
    stream >> i;
    packet._sessionPresent = (i & 0x01) != 0;
    packet._connectAcknowledgeBitsValid = (i & 0xfe) == 0;

    stream >> i;
    packet._connectReturnCode = static_cast<ConnectReturnCodeType>(i);

    buffer.close();

    return packet;
}

QMQTT::Frame QMQTT::ConnackPacket::toFrame() const
{
    Frame frame;

    frame._header = static_cast<quint8>(type()) << 4;

    QBuffer buffer(&frame._data);
    buffer.open(QIODevice::WriteOnly);
    QDataStream stream(&buffer);
    stream << static_cast<quint8>(_sessionPresent ? 1 : 0);
    stream << static_cast<quint8>(_connectReturnCode);
    buffer.close();

    return frame;
}
