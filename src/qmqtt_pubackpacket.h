#ifndef QMQTT_PUBACK_PACKET_H
#define QMQTT_PUBACK_PACKET_H

#include "qmqtt_abstractpacket.h"
#include <QDataStream>

namespace QMQTT
{

class PubackPacket : public AbstractPacket
{
public:
    PubackPacket();
    virtual ~PubackPacket();

    bool isValid() const;
    PacketType type() const;

    quint16 packetIdentifier() const;
    void setPacketIdentifier(const quint16 packetIdentifier);

protected:
    quint16 _packetIdentifier;

    qint64 calculateRemainingLengthFromData() const;

private:
    friend QDataStream& operator>>(QDataStream& stream, PubackPacket& packet);
    friend QDataStream& operator<<(QDataStream& stream, const PubackPacket& packet);
};

QDataStream& operator>>(QDataStream& stream, PubackPacket& packet);
QDataStream& operator<<(QDataStream& stream, const PubackPacket& packet);

} // end namespace QMQTT

#endif // QMQTT_PUBACK_PACKET_H
