/*
 * qmqtt_disconnectpacket.cpp - qmqtt disconnect packet
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
#include "qmqtt_disconnectpacket.h"
#include <QDataStream>
#include <QBuffer>

QMQTT::DisconnectPacket::DisconnectPacket()
    : _headerReservedBitsValid(true)
{
}

QMQTT::DisconnectPacket::~DisconnectPacket()
{
}

QMQTT::PacketType QMQTT::DisconnectPacket::type() const
{
    return QMQTT::DisconnectType;
}

bool QMQTT::DisconnectPacket::isValid() const
{
    if (!_headerReservedBitsValid)
    {
        return false;
    }

    return true;
}

QMQTT::Frame QMQTT::DisconnectPacket::toFrame() const
{
    Frame frame;

    frame._header = static_cast<quint8>(type()) << 4;

    return frame;
}

QMQTT::DisconnectPacket QMQTT::DisconnectPacket::fromFrame(const Frame& frame)
{
    DisconnectPacket packet;

    packet._headerReservedBitsValid = (frame._header & 0x0f) == 0;

    return packet;
}
