#ifndef QMQTT_PUBLISH_PACKET_H
#define QMQTT_PUBLISH_PACKET_H

#include "qmqtt_abstractpacket.h"
#include <QDataStream>
//#include <qglobal.h>
#include <QString>

namespace QMQTT
{

class PublishPacket : public AbstractPacket
{
public:
    PublishPacket();
    virtual ~PublishPacket();

    bool isValid() const;
    PacketType type() const;

    bool dupFlag() const;
    void setDupFlag(const bool dupFlag);
    quint8 qos() const;
    void setQos(const quint8 qos);
    bool retainFlag() const;
    void setRetainFlag(const bool retainFlag);
    QString topicName() const;
    void setTopicName(const QString& topicName);
    quint16 packetIdentifier() const;
    void setPacketIdentifier(const quint16 packetIdentifier);
    QByteArray payload() const;
    void setPayload(const QByteArray& payload);

protected:
    QString _topicName;
    quint16 _packetIdentifier;
    QByteArray _payload;

    qint64 calculateRemainingLengthFromData() const;

private:
    friend QDataStream& operator>>(QDataStream& stream, PublishPacket& packet);
    friend QDataStream& operator<<(QDataStream& stream, const PublishPacket& packet);
};

QDataStream& operator>>(QDataStream& stream, PublishPacket& packet);
QDataStream& operator<<(QDataStream& stream, const PublishPacket& packet);

} // end namespace QMQTT

#endif // QMQTT_PUBLISH_PACKET_H
