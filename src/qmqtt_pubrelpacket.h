#ifndef QMQTT_PUBREL_PACKET_H
#define QMQTT_PUBREL_PACKET_H

#include "qmqtt_abstractpacket.h"
#include <QDataStream>

namespace QMQTT
{

class PubrelPacket : public AbstractPacket
{
public:
    PubrelPacket();
    virtual ~PubrelPacket();

    bool isValid() const;
    PacketType type() const;

    quint16 packetIdentifier() const;
    void setPacketIdentifier(const quint16 packetIdentifier);

protected:
    quint16 _packetIdentifier;

    qint64 calculateRemainingLengthFromData() const;

private:
    friend QDataStream& operator>>(QDataStream& stream, PubrelPacket& packet);
    friend QDataStream& operator<<(QDataStream& stream, const PubrelPacket& packet);
};

QDataStream& operator>>(QDataStream& stream, PubrelPacket& packet);
QDataStream& operator<<(QDataStream& stream, const PubrelPacket& packet);

} // end namespace QMQTT

#endif // QMQTT_PUBREL_PACKET_H
