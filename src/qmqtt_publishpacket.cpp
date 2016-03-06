/*
 * qmqtt_publishpacket.cpp - qmqtt publish packet
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
#include "qmqtt_publishpacket.h"
#include <QDataStream>
#include <QBuffer>

const quint8 DEFAULT_FIXED_HEADER = QMQTT::PublishType << 4;
const quint8 DUP_FLAG_MASK = 0x08;
const quint8 QOS_MASK = 0x06;
const quint8 RETAIN_FLAG_MASK = 0x01;

QMQTT::PublishPacket::PublishPacket()
    : _packetIdentifier(0)
    , _dupFlag(false)
    , _qos(Qos0)
    , _retainFlag(false)
{
}

QMQTT::PublishPacket::~PublishPacket()
{
}

QMQTT::PacketType QMQTT::PublishPacket::type() const
{
    return QMQTT::PublishType;
}

bool QMQTT::PublishPacket::isValid() const
{
    if (_qos == Qos0 && _dupFlag)
    {
        return false;
    }

    if (static_cast<quint8>(_qos) > static_cast<quint8>(Qos2))
    {
        return false;
    }

    if (_topicName.contains(QRegExp("[#+]")))
    {
        return false;
    }

    if (_qos != Qos0 && _packetIdentifier == 0)
    {
        return false;
    }

    return true;
}

bool QMQTT::PublishPacket::dupFlag() const
{
    return _dupFlag;
}

void QMQTT::PublishPacket::setDupFlag(const bool dupFlag)
{
    _dupFlag = dupFlag;
}

QMQTT::QosType QMQTT::PublishPacket::qos() const
{
    return _qos;
}

void QMQTT::PublishPacket::setQos(const QosType qos)
{
    _qos = qos;
}

bool QMQTT::PublishPacket::retainFlag() const
{
    return _retainFlag;
}

void QMQTT::PublishPacket::setRetainFlag(const bool retainFlag)
{
    _retainFlag = retainFlag;
}

QString QMQTT::PublishPacket::topicName() const
{
    return _topicName;
}

void QMQTT::PublishPacket::setTopicName(const QString& topicName)
{
    _topicName = topicName;
}

quint16 QMQTT::PublishPacket::packetIdentifier() const
{
    return _packetIdentifier;
}

void QMQTT::PublishPacket::setPacketIdentifier(const quint16 packetIdentifier)
{
    _packetIdentifier = packetIdentifier;
}

QByteArray QMQTT::PublishPacket::payload() const
{
    return _payload;
}

void QMQTT::PublishPacket::setPayload(const QByteArray& payload)
{
    _payload = payload;
}

QMQTT::Frame QMQTT::PublishPacket::toFrame() const
{
    Frame frame;

    frame._header =
            (static_cast<quint8>(type()) << 4)
             | ((_dupFlag ? 1 : 0) << 3)
             | (static_cast<quint8>(_qos) << 1)
             | (_retainFlag ? 1 : 0);

    QBuffer buffer(&frame._data);
    buffer.open(QIODevice::WriteOnly);
    QDataStream stream(&buffer);

    writeString(stream, _topicName);
    stream << _packetIdentifier;
    stream.writeRawData(_payload.constData(), _payload.size());

    buffer.close();

    return frame;
}

QMQTT::PublishPacket QMQTT::PublishPacket::fromFrame(const Frame& frame)
{
    PublishPacket packet;

    packet._dupFlag = (frame._header & 0x08) != 0;
    packet._qos = static_cast<QosType>((frame._header & 0x06) >> 1);
    packet._retainFlag = (frame._header & 0x01) != 0;

    QBuffer buffer;
    buffer.setData(frame._data);
    buffer.open(QIODevice::ReadOnly);
    QDataStream stream(&buffer);

    packet._topicName = readString(stream);
    stream >> packet._packetIdentifier;

    int payloadLength = buffer.size() - buffer.pos();
    packet._payload.resize(payloadLength);
    stream.readRawData(packet._payload.data(), payloadLength);

    buffer.close();

    return packet;
}
