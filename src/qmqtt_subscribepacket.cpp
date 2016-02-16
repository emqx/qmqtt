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
    foreach (const Subscription& subscription, _subscriptionList)
    {
        remainingLength += subscription._topicFilter.size() + 3;
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

    if (_subscriptionList.isEmpty())
    {
        return false;
    }

    foreach (const Subscription& subscription, _subscriptionList)
    {
        if (subscription._topicFilter.isEmpty())
        {
            return false;
        }

        if (subscription._requestedQos > 2)
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

QMQTT::SubscriptionList QMQTT::SubscribePacket::subscriptionList() const
{
    return _subscriptionList;
}

void QMQTT::SubscribePacket::setSubscriptionList(const SubscriptionList subscriptionList)
{
    _subscriptionList = subscriptionList;
}

QDataStream& QMQTT::operator>>(QDataStream& stream, SubscribePacket& packet)
{
    stream >> packet._fixedHeader;
    qint64 remainingLength = packet.readRemainingLength(stream);
    stream >> packet._packetIdentifier;
    remainingLength -= 2;
    packet._subscriptionList.clear();
    while (remainingLength > 0)
    {
        Subscription subscription;
        subscription._topicFilter = packet.readString(stream);
        stream >> subscription._requestedQos;

        packet._subscriptionList.append(subscription);
        remainingLength -= subscription._topicFilter.size() + 3;
    }
    return stream;
}

QDataStream& QMQTT::operator<<(QDataStream& stream, const SubscribePacket& packet)
{
    stream << packet._fixedHeader;
    packet.writeRemainingLength(stream);
    stream << packet._packetIdentifier;
    foreach (const Subscription& subscription, packet._subscriptionList)
    {
        packet.writeString(stream, subscription._topicFilter);
        stream << subscription._requestedQos;
    }
    return stream;
}
