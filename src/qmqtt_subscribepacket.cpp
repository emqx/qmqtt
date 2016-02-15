#include "qmqtt_subscribepacket.h"
#include <QDataStream>

const quint8 DEFAULT_FIXED_HEADER = (QMQTT::SubscribeType << 4) | 0x02;

QMQTT::SubscribePacket::SubscribePacket()
    : AbstractPacket(DEFAULT_FIXED_HEADER)
    , _packetIdentifier(0)
{
}

QMQTT::SubscribePacket::~SubscribePacket()
{
}

QMQTT::PacketType QMQTT::SubscribePacket::type() const
{
    return QMQTT::SubscribeType;
}

qint64 QMQTT::SubscribePacket::calculateRemainingLengthFromData() const
{
    quint64 remainingLength = 2;
    foreach (const Topic& topic, _topicList)
    {
        remainingLength += topic._name.size() + 3;
    }
    return remainingLength;
}

bool QMQTT::SubscribePacket::isValid() const
{
    if (_fixedHeader != DEFAULT_FIXED_HEADER)
    {
        return false;
    }

    // todo: re-check all wildcards

    if (_topicList.isEmpty())
    {
        return false;
    }

    foreach (const Topic& topic, _topicList)
    {
        if (topic._name.isEmpty())
        {
            return false;
        }

        if (topic._requestedQos > 2)
        {
            return false;
        }
    }

    return true;
}

quint16 QMQTT::SubscribePacket::packetIdentifier() const
{
    return _packetIdentifier;
}

void QMQTT::SubscribePacket::setPacketIdentifier(const quint16 packetIdentifier)
{
    _packetIdentifier = packetIdentifier;
}

QMQTT::TopicList QMQTT::SubscribePacket::topicList() const
{
    return _topicList;
}

void QMQTT::SubscribePacket::setTopicList(const TopicList subscriptionList)
{
    _topicList = subscriptionList;
}

QDataStream& QMQTT::operator>>(QDataStream& stream, SubscribePacket& packet)
{
    stream >> packet._fixedHeader;
    qint64 remainingLength = packet.readRemainingLength(stream);
    stream >> packet._packetIdentifier;
    remainingLength -= 2;
    packet._topicList.clear();
    while (remainingLength > 0)
    {
        Topic topic;
        topic._name = packet.readString(stream);
        stream >> topic._requestedQos;

        packet._topicList.append(topic);
        remainingLength -= topic._name.size() + 3;
    }
    return stream;
}

QDataStream& QMQTT::operator<<(QDataStream& stream, const SubscribePacket& packet)
{
    stream << packet._fixedHeader;
    packet.writeRemainingLength(stream);
    stream << packet._packetIdentifier;
    foreach (const Topic& topic, packet._topicList)
    {
        packet.writeString(stream, topic._name);
        stream << topic._requestedQos;
    }
    return stream;
}
