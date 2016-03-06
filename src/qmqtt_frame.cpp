/*
 * qmqtt_frame.cpp - qmqtt frame
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
#include <QDataStream>
#include <QLoggingCategory>
#include "qmqtt_frame.h"

Q_LOGGING_CATEGORY(frame, "qmqtt.frame")

QMQTT::Frame::Frame()
    : _header(0)
{
}

QMQTT::Frame::Frame(const quint8& header)
    : _header(header)
{
}

QMQTT::Frame::Frame(const quint8& header, const QByteArray& data)
    : _header(header)
    , _data(data)
{
}

QMQTT::Frame::Frame(const Frame& other)
    : _header(other._header)
    , _data(other._data)
{
}

QMQTT::Frame& QMQTT::Frame::operator=(const Frame& other)
{
    if (this != &other)
    {
        _header = other._header;
        _data = other._data;
    }
    return *this;
}

bool QMQTT::Frame::operator==(const Frame& other) const
{
    return _header == other._header && _data == other._data;
}

bool QMQTT::Frame::operator!=(const Frame& other) const
{
    return _header != other._header || _data != other._data;
}

QMQTT::Frame::~Frame()
{
}

int QMQTT::Frame::readRemainingLength(QDataStream& stream)
{
    int multiplier = 1;
    int remainingLength = 0;
    quint8 encodedByte = 0;
    do {
        stream >> encodedByte;
        remainingLength += (encodedByte & 127) * multiplier;
        multiplier *= 128;
    } while ((encodedByte & 128) != 0);
    return remainingLength;
}

void QMQTT::Frame::writeRemainingLength(QDataStream& stream, int remainingLength)
{
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

QDataStream& QMQTT::operator>>(QDataStream& stream, Frame& frame)
{
    stream >> frame._header;
    int remainingLength = frame.readRemainingLength(stream);
    frame._data.resize(remainingLength);
    stream.readRawData(frame._data.data(), remainingLength);

    return stream;
}

QDataStream& QMQTT::operator<<(QDataStream& stream, const Frame& frame)
{
    stream << frame._header;
    frame.writeRemainingLength(stream, frame._data.size());
    stream.writeRawData(frame._data.constData(), frame._data.size());

    return stream;
}
