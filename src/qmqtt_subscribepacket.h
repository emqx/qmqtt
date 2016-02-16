#ifndef QMQTT_SUBSCRIBE_PACKET_H
#define QMQTT_SUBSCRIBE_PACKET_H

#include "qmqtt_abstractpacket.h"
#include "qmqtt_subscription.h"
#include <QDataStream>

namespace QMQTT
{

class SubscribePacket : public AbstractPacket
{
public:
    SubscribePacket();
    virtual ~SubscribePacket();

    bool isValid() const;
    PacketType type() const;

    quint16 packetIdentifier() const;
    void setPacketIdentifier(const quint16 packetIdentifier);

    SubscriptionList subscriptionList() const;
    void setSubscriptionList(const SubscriptionList subscriptionList);

protected:
    quint16 _packetIdentifier;
    SubscriptionList _subscriptionList;

    qint64 calculateRemainingLengthFromData() const;

private:
    friend QDataStream& operator>>(QDataStream& stream, SubscribePacket& packet);
    friend QDataStream& operator<<(QDataStream& stream, const SubscribePacket& packet);
};

QDataStream& operator>>(QDataStream& stream, SubscribePacket& packet);
QDataStream& operator<<(QDataStream& stream, const SubscribePacket& packet);

} // end namespace QMQTT

#endif // QMQTT_SUBSCRIBE_PACKET_H
