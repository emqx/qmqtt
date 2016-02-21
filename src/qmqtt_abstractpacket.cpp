/*
 * qmqtt_abstractpacket.cpp - qmqtt abstract packet
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
#include "qmqtt_abstractpacket.h"
#include <QDataStream>

QMQTT::AbstractPacket::AbstractPacket(const quint8 fixedHeader)
    : _fixedHeader(fixedHeader)
{
}

QMQTT::AbstractPacket::~AbstractPacket()
{
}

qint64 QMQTT::AbstractPacket::readRemainingLength(QDataStream& stream) const
{
    int multiplier = 1;
    qint64 remainingLength = 0;
    quint8 encodedByte = 0;
    do {
        stream >> encodedByte;
        remainingLength += (encodedByte & 127) * multiplier;
        multiplier *= 128;
    } while ((encodedByte & 128) != 0);
    return remainingLength;
}

void QMQTT::AbstractPacket::writeRemainingLength(QDataStream& stream) const
{
    qint64 remainingLength = calculateRemainingLengthFromData();
    quint8 encodedByte = 0;
    do {
        encodedByte = remainingLength % 128;
        remainingLength /= 128;
        if (remainingLength > 0)
        {
            encodedByte |= 128;
        }
        stream << encodedByte;
    } while (remainingLength > 0);
}

QString QMQTT::AbstractPacket::readString(QDataStream& stream)
{
    quint16 length;
    stream >> length;
    QByteArray byteArray;
    byteArray.resize(length);
    stream.readRawData(byteArray.data(), length);
    return QString::fromUtf8(byteArray);
}

void QMQTT::AbstractPacket::writeString(QDataStream& stream, const QString& string) const
{
    QByteArray byteArray(string.toUtf8());
    stream << static_cast<quint16>(byteArray.size());
    stream.writeRawData(byteArray.constData(), string.size());
}
