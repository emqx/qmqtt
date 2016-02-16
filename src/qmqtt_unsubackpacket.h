#ifndef QMQTT_UNSUBACK_PACKET_H
#define QMQTT_UNSUBACK_PACKET_H

#include "qmqtt_abstractpacket.h"
#include <QDataStream>

namespace QMQTT
{

class UnsubackPacket : public AbstractPacket
{
public:
    UnsubackPacket();
    virtual ~UnsubackPacket();

    bool isValid() const;
    PacketType type() const;

    quint16 packetIdentifier() const;
    void setPacketIdentifier(const quint16 packetIdentifier);

protected:
    quint16 _packetIdentifier;

    qint64 calculateRemainingLengthFromData() const;

private:
    friend QDataStream& operator>>(QDataStream& stream, UnsubackPacket& packet);
    friend QDataStream& operator<<(QDataStream& stream, const UnsubackPacket& packet);
};

QDataStream& operator>>(QDataStream& stream, UnsubackPacket& packet);
QDataStream& operator<<(QDataStream& stream, const UnsubackPacket& packet);

} // end namespace QMQTT

#endif // QMQTT_UNSUBACK_PACKET_H
