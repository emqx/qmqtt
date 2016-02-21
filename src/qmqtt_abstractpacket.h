/*
 * qmqtt_abstractpacket.h - qmqtt abstract packet header
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
#ifndef QMQTT_ABSTRACT_PACKET_H
#define QMQTT_ABSTRACT_PACKET_H

#include <qglobal.h>

namespace QMQTT
{

enum PacketType
{
    ConnectType     = 0x01,
    ConnackType     = 0x02,
    PublishType     = 0x03,
    PubackType      = 0x04,
    PubrecType      = 0x05,
    PubrelType      = 0x06,
    PubcompType     = 0x07,
    SubscribeType   = 0x08,
    SubackType      = 0x09,
    UnsubscribeType = 0x0a,
    UnsubackType    = 0x0b,
    PingreqType     = 0x0c,
    PingrespType    = 0x0d,
    DisconnectType  = 0x0e
};

class AbstractPacket
{
public:
    AbstractPacket(const quint8 fixedHeader);
    virtual ~AbstractPacket();

    virtual bool isValid() const = 0;
    virtual PacketType type() const = 0;
    virtual qint64 calculateRemainingLengthFromData() const = 0;

protected:
    qint64 readRemainingLength(QDataStream& stream) const;
    void writeRemainingLength(QDataStream& stream) const;
    QString readString(QDataStream& stream);
    void writeString(QDataStream& stream, const QString& string) const;

    quint8 _fixedHeader;
};

} // end namespace QMQTT

#endif // QMQTT_ABSTRACT_PACKET_H
