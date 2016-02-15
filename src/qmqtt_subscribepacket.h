#ifndef QMQTT_SUBSCRIBE_PACKET_H
#define QMQTT_SUBSCRIBE_PACKET_H

#include "qmqtt_abstractpacket.h"
#include <QDataStream>

namespace QMQTT
{

class Topic
{
public:
    QString _name;
    quint8 _requestedQos;
};

typedef QList<Topic> TopicList;

class SubscribePacket : public AbstractPacket
{
public:
    SubscribePacket();
    virtual ~SubscribePacket();

    bool isValid() const;
    PacketType type() const;

    quint16 packetIdentifier() const;
    void setPacketIdentifier(const quint16 packetIdentifier);

    TopicList topicList() const;
    void setTopicList(const TopicList topicList);

protected:
    quint16 _packetIdentifier;
    TopicList _topicList;

    qint64 calculateRemainingLengthFromData() const;

private:
    friend QDataStream& operator>>(QDataStream& stream, SubscribePacket& packet);
    friend QDataStream& operator<<(QDataStream& stream, const SubscribePacket& packet);
};

QDataStream& operator>>(QDataStream& stream, SubscribePacket& packet);
QDataStream& operator<<(QDataStream& stream, const SubscribePacket& packet);

} // end namespace QMQTT

#endif // QMQTT_SUBSCRIBE_PACKET_H
