#include "qmqtt_publishpacket.h"
#include <QDataStream>
#include <QDebug>

const quint8 DEFAULT_FIXED_HEADER = QMQTT::PublishType << 4;
const quint8 DUP_FLAG_MASK = 0x08;
const quint8 QOS_MASK = 0x06;
const quint8 RETAIN_FLAG_MASK = 0x01;

QMQTT::PublishPacket::PublishPacket()
    : AbstractPacket(DEFAULT_FIXED_HEADER)
    , _packetIdentifier(0)
{
}

QMQTT::PublishPacket::~PublishPacket()
{
}

QMQTT::PacketType QMQTT::PublishPacket::type() const
{
    return QMQTT::PublishType;
}

qint64 QMQTT::PublishPacket::calculateRemainingLengthFromData() const
{
    qint64 remainingLength = _topicName.size() + _payload.size() + 2;
    if ((_fixedHeader & QOS_MASK) != 0)
    {
        remainingLength += 2;
    }
    return remainingLength;
}

bool QMQTT::PublishPacket::isValid() const
{
    if (qos() == 0 && dupFlag())
    {
        return false;
    }

    if (qos() == 3)
    {
        return false;
    }

    if (topicName().contains(QRegExp("[*?]")))
    {
        return false;
    }

    if ((qos() == 1 || qos() == 2) && packetIdentifier() != 0)
    {
        return false;
    }

    return true;
}

bool QMQTT::PublishPacket::dupFlag() const
{
    return (_fixedHeader & DUP_FLAG_MASK) != 0;
}

void QMQTT::PublishPacket::setDupFlag(const bool dupFlag)
{
    _fixedHeader &= ~DUP_FLAG_MASK;
    if (dupFlag)
    {
        _fixedHeader |= DUP_FLAG_MASK;
    }
}

quint8 QMQTT::PublishPacket::qos() const
{
    return (_fixedHeader & QOS_MASK) >> 1;
}

void QMQTT::PublishPacket::setQos(const quint8 qos)
{
    _fixedHeader &= ~QOS_MASK;
    _fixedHeader |= (qos << 1);
}

bool QMQTT::PublishPacket::retainFlag() const
{
    return (_fixedHeader & RETAIN_FLAG_MASK) != 0;
}

void QMQTT::PublishPacket::setRetainFlag(const bool retainFlag)
{
    _fixedHeader &= ~RETAIN_FLAG_MASK;
    if (retainFlag)
    {
        _fixedHeader |= RETAIN_FLAG_MASK;
    }
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

QDataStream& QMQTT::operator>>(QDataStream& stream, PublishPacket& packet)
{
    stream >> packet._fixedHeader;
    qint64 remainingLength = packet.readRemainingLength(stream);
    packet._topicName = packet.readString(stream);    
    if ((packet._fixedHeader & QOS_MASK) != 0)
    {
        stream >> packet._packetIdentifier;
    }
    qint64 payloadLength = remainingLength - packet._topicName.size() - 4;
    packet._payload.resize(payloadLength);
    stream.readRawData(packet._payload.data(), payloadLength);

    return stream;
}

QDataStream& QMQTT::operator<<(QDataStream& stream, const PublishPacket& packet)
{
    stream << packet._fixedHeader;
    packet.writeRemainingLength(stream);
    packet.writeString(stream, packet._topicName);
    if ((packet._fixedHeader & QOS_MASK) != 0)
    {
        stream << packet._packetIdentifier;
    }
    stream.writeRawData(packet._payload.constData(), packet._payload.size());

    return stream;
}
