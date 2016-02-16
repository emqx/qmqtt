#include "qmqtt_unsubscribepacket.h"
#include <QDataStream>

const quint8 DEFAULT_FIXED_HEADER = (QMQTT::UnsubscribeType << 4) | 0x02;

QMQTT::UnsubscribePacket::UnsubscribePacket()
    : AbstractPacket(DEFAULT_FIXED_HEADER)
    , _packetIdentifier(0)
{
}

QMQTT::UnsubscribePacket::~UnsubscribePacket()
{
}

QMQTT::PacketType QMQTT::UnsubscribePacket::type() const
{
    return QMQTT::UnsubscribeType;
}

qint64 QMQTT::UnsubscribePacket::calculateRemainingLengthFromData() const
{
    quint64 remainingLength = 2;
    foreach (const QString& topicFilter, _topicFilterList)
    {
        remainingLength += topicFilter.size() + 2;
    }
    return remainingLength;
}

bool QMQTT::UnsubscribePacket::isValid() const
{
    if (_fixedHeader != DEFAULT_FIXED_HEADER)
    {
        return false;
    }

    // todo: re-check all wildcards

    if (_topicFilterList.isEmpty())
    {
        return false;
    }

    foreach (const QString& topicFilter, _topicFilterList)
    {
        if (topicFilter.isEmpty())
        {
            return false;
        }
    }

    return true;
}

quint16 QMQTT::UnsubscribePacket::packetIdentifier() const
{
    return _packetIdentifier;
}

void QMQTT::UnsubscribePacket::setPacketIdentifier(const quint16 packetIdentifier)
{
    _packetIdentifier = packetIdentifier;
}

QStringList QMQTT::UnsubscribePacket::topicFilterList() const
{
    return _topicFilterList;
}

void QMQTT::UnsubscribePacket::setTopicFilterList(const QStringList& topicFilterList)
{
    _topicFilterList = topicFilterList;
}

QDataStream& QMQTT::operator>>(QDataStream& stream, UnsubscribePacket& packet)
{
    stream >> packet._fixedHeader;
    qint64 remainingLength = packet.readRemainingLength(stream);
    stream >> packet._packetIdentifier;
    remainingLength -= 2;
    packet._topicFilterList.clear();
    while (remainingLength > 0)
    {
        QString topicFilter = packet.readString(stream);
        packet._topicFilterList.append(topicFilter);
        remainingLength -= topicFilter.size() + 2;
    }
    return stream;
}

QDataStream& QMQTT::operator<<(QDataStream& stream, const UnsubscribePacket& packet)
{
    stream << packet._fixedHeader;
    packet.writeRemainingLength(stream);
    stream << packet._packetIdentifier;
    foreach (const QString& topicFilter, packet._topicFilterList)
    {
        packet.writeString(stream, topicFilter);
    }
    return stream;
}
