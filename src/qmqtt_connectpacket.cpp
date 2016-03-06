/*
 * qmqtt_connectpacket.cpp - qmqtt connect packet
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
#include "qmqtt_connectpacket.h"
#include <QRegExp>
#include <QBuffer>

namespace
{
    const quint8 DEFAULT_HEADER = QMQTT::ConnectType << 4;
    const QString SUPPORTED_PROTOCOL = QStringLiteral("MQTT");
    const quint8 SUPPORTED_PROTOCOL_LEVEL = 0x04;
    const bool DEFAULT_CLEAN_SESSION = false;
    const QMQTT::QosType DEFAULT_WILL_QOS_TYPE = QMQTT::Qos0;
    const bool DEFAULT_WILL_RETAIN = false;
    const quint16 DEFAULT_KEEP_ALIVE_INTERVAL = 300;
}

QMQTT::ConnectPacket::ConnectPacket()
    : _protocol(SUPPORTED_PROTOCOL)
    , _protocolLevel(SUPPORTED_PROTOCOL_LEVEL)
    , _cleanSession(DEFAULT_CLEAN_SESSION)
    , _willQos(DEFAULT_WILL_QOS_TYPE)
    , _willRetain(DEFAULT_WILL_RETAIN)
    , _keepAlive(DEFAULT_KEEP_ALIVE_INTERVAL)
    , _headerReservedBitsValid(true)
{
}

QMQTT::ConnectPacket::~ConnectPacket()
{
}

QMQTT::PacketType QMQTT::ConnectPacket::type() const
{
    return ConnectType;
}

QString QMQTT::ConnectPacket::protocol() const
{
    return _protocol;
}

quint8 QMQTT::ConnectPacket::protocolLevel() const
{
    return _protocolLevel;
}

quint16 QMQTT::ConnectPacket::keepAlive() const
{
    return _keepAlive;
}

void QMQTT::ConnectPacket::setKeepAlive(const quint16 keepAlive)
{
    _keepAlive = keepAlive;
}

bool QMQTT::ConnectPacket::cleanSession() const
{
    return _cleanSession;
}

void QMQTT::ConnectPacket::setCleanSession(const bool cleanSession)
{
    _cleanSession = cleanSession;
}

QMQTT::QosType QMQTT::ConnectPacket::willQos() const
{
    return _willQos;
}

void QMQTT::ConnectPacket::setWillQos(const QosType willQos)
{
    _willQos = willQos;
}

bool QMQTT::ConnectPacket::willRetain() const
{
    return _willRetain;
}

void QMQTT::ConnectPacket::setWillRetain(const bool willRetain)
{
    _willRetain = willRetain;
}

QString QMQTT::ConnectPacket::willTopic() const
{
    return _willTopic;
}

void QMQTT::ConnectPacket::setWillTopic(const QString& willTopic)
{
    _willTopic = willTopic;
}

QString QMQTT::ConnectPacket::willMessage() const
{
    return _willMessage;
}

void QMQTT::ConnectPacket::setWillMessage(const QString& willMessage)
{
    _willMessage = willMessage;
}

QString QMQTT::ConnectPacket::clientIdentifier() const
{
    return _clientIdentifier;
}

void QMQTT::ConnectPacket::setClientIdentifier(const QString& clientIdentifier)
{
    _clientIdentifier = clientIdentifier;
}

QString QMQTT::ConnectPacket::userName() const
{
    return _userName;
}

void QMQTT::ConnectPacket::setUserName(const QString& userName)
{
    _userName = userName;
}

QString QMQTT::ConnectPacket::password() const
{
    return _password;
}

void QMQTT::ConnectPacket::setPassword(const QString& password)
{
    _password = password;
}

bool QMQTT::ConnectPacket::isValid() const
{
    if (!_headerReservedBitsValid)
    {
        return false;
    }

    if (_protocol != SUPPORTED_PROTOCOL)
    {
        return false;
    }

    if (_protocolLevel != SUPPORTED_PROTOCOL_LEVEL)
    {
        return false;
    }

    if (!_willTopic.isEmpty())
    {
        if (static_cast<quint8>(_willQos) > 2)
        {
            return false;
        }
    }
    else
    {
        if (_willQos != Qos0)
        {
            return false;
        }
        if (_willRetain)
        {
            return false;
        }
    }

    if (_userName.isEmpty() && !_password.isEmpty())
    {
        return false;
    }

    QRegExp re("[\\da-zA-Z]{1,23}");
    if (!re.exactMatch(_clientIdentifier))
    {
        return false;
    }

    return true;
}

QMQTT::ConnectPacket QMQTT::ConnectPacket::fromFrame(const Frame& frame)
{
    ConnectPacket packet;

    packet._headerReservedBitsValid = (frame._header & 0x0f) == 0;

    QBuffer buffer;
    buffer.setData(frame._data);
    buffer.open(QIODevice::ReadOnly);
    QDataStream stream(&buffer);

    packet._protocol = packet.readString(stream);
    stream >> packet._protocolLevel;

    quint8 connectFlags = 0x00;
    stream >> connectFlags;
    packet._cleanSession = (connectFlags & 0x02) != 0;
    bool willFlag = (connectFlags & 0x04) != 0;
    packet._willQos = static_cast<QosType>((connectFlags & 0x18) >> 3);
    packet._willRetain = (connectFlags & 0x20) != 0;
    bool passwordFlag = (connectFlags & 0x40) != 0;
    bool userNameFlag = (connectFlags & 0x80) != 0;

    stream >> packet._keepAlive;

    packet._clientIdentifier = packet.readString(stream);
    packet._willTopic.clear();
    packet._willMessage.clear();
    if (willFlag)
    {
        packet._willTopic = packet.readString(stream);
        packet._willMessage = packet.readString(stream);
    }
    packet._userName.clear();
    if (userNameFlag)
    {
        packet._userName = packet.readString(stream);
    }
    packet._password.clear();
    if (passwordFlag)
    {
        packet._password = packet.readString(stream);
    }

    buffer.close();

    return packet;
}

QMQTT::Frame QMQTT::ConnectPacket::toFrame() const
{
    Frame frame;

    frame._header = static_cast<quint8>(type()) << 4;

    QBuffer buffer(&frame._data);
    buffer.open(QIODevice::WriteOnly);
    QDataStream stream(&buffer);

    writeString(stream, _protocol);
    stream << _protocolLevel;

    quint8 connectFlags = 0x00;
    connectFlags |= (_cleanSession?1:0) << 1;
    connectFlags |= (_willTopic.isEmpty()?0:1) << 2;
    connectFlags |= _willQos << 3;
    connectFlags |= (_willRetain?1:0) << 5;
    connectFlags |= (_password.isEmpty()?0:1) << 6;
    connectFlags |= (_userName.isEmpty()?0:1) << 7;
    stream << connectFlags;

    stream << _keepAlive;

    writeString(stream, _clientIdentifier);
    if (!_willTopic.isEmpty())
    {
        writeString(stream, _willTopic);
        writeString(stream, _willMessage);
    }
    if (!_userName.isEmpty())
    {
        writeString(stream, _userName);
    }
    if (!_password.isEmpty())
    {
        writeString(stream, _password);
    }

    buffer.close();
    return frame;
}
