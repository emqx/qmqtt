#ifndef QMQTT_PUBCOMP_PACKET_H
#define QMQTT_PUBCOMP_PACKET_H

#include "qmqtt_abstractpacket.h"
#include <QDataStream>

namespace QMQTT
{

class PubcompPacket : public AbstractPacket
{
public:
    PubcompPacket();
    virtual ~PubcompPacket();

    bool isValid() const;
    PacketType type() const;

    quint16 packetIdentifier() const;
    void setPacketIdentifier(const quint16 packetIdentifier);

protected:
    quint16 _packetIdentifier;

    qint64 calculateRemainingLengthFromData() const;

private:
    friend QDataStream& operator>>(QDataStream& stream, PubcompPacket& packet);
    friend QDataStream& operator<<(QDataStream& stream, const PubcompPacket& packet);
};

QDataStream& operator>>(QDataStream& stream, PubcompPacket& packet);
QDataStream& operator<<(QDataStream& stream, const PubcompPacket& packet);

} // end namespace QMQTT

#endif // QMQTT_PUBCOMP_PACKET_H
