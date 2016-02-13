#ifndef QMQTT_PUBREC_PACKET_H
#define QMQTT_PUBREC_PACKET_H

#include "qmqtt_abstractpacket.h"
#include <QDataStream>

namespace QMQTT
{

class PubrecPacket : public AbstractPacket
{
public:
    PubrecPacket();
    virtual ~PubrecPacket();

    bool isValid() const;
    PacketType type() const;

    quint16 packetIdentifier() const;
    void setPacketIdentifier(const quint16 packetIdentifier);

protected:
    quint16 _packetIdentifier;

    qint64 calculateRemainingLengthFromData() const;

private:
    friend QDataStream& operator>>(QDataStream& stream, PubrecPacket& packet);
    friend QDataStream& operator<<(QDataStream& stream, const PubrecPacket& packet);
};

QDataStream& operator>>(QDataStream& stream, PubrecPacket& packet);
QDataStream& operator<<(QDataStream& stream, const PubrecPacket& packet);

} // end namespace QMQTT

#endif // QMQTT_PUBREC_PACKET_H
