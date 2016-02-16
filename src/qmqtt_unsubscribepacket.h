#ifndef QMQTT_UNSUBSCRIBE_PACKET_H
#define QMQTT_UNSUBSCRIBE_PACKET_H

#include "qmqtt_abstractpacket.h"
#include "qmqtt_subscription.h"
#include <QDataStream>

namespace QMQTT
{

class UnsubscribePacket : public AbstractPacket
{
public:
    UnsubscribePacket();
    virtual ~UnsubscribePacket();

    bool isValid() const;
    PacketType type() const;

    quint16 packetIdentifier() const;
    void setPacketIdentifier(const quint16 packetIdentifier);

    QStringList topicFilterList() const;
    void setTopicFilterList(const QStringList& topicFilterList);

protected:
    quint16 _packetIdentifier;
    QStringList _topicFilterList;

    qint64 calculateRemainingLengthFromData() const;

private:
    friend QDataStream& operator>>(QDataStream& stream, UnsubscribePacket& packet);
    friend QDataStream& operator<<(QDataStream& stream, const UnsubscribePacket& packet);
};

QDataStream& operator>>(QDataStream& stream, UnsubscribePacket& packet);
QDataStream& operator<<(QDataStream& stream, const UnsubscribePacket& packet);

} // end namespace QMQTT

#endif // QMQTT_UNSUBSCRIBE_PACKET_H
