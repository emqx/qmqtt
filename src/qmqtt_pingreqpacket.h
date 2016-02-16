#ifndef QMQTT_PINGREQ_PACKET_H
#define QMQTT_PINGREQ_PACKET_H

#include "qmqtt_abstractpacket.h"
#include <QDataStream>

namespace QMQTT
{

class PingreqPacket : public AbstractPacket
{
public:
    PingreqPacket();
    virtual ~PingreqPacket();

    bool isValid() const;
    PacketType type() const;

protected:
    qint64 calculateRemainingLengthFromData() const;

private:
    friend QDataStream& operator>>(QDataStream& stream, PingreqPacket& packet);
    friend QDataStream& operator<<(QDataStream& stream, const PingreqPacket& packet);
};

QDataStream& operator>>(QDataStream& stream, PingreqPacket& packet);
QDataStream& operator<<(QDataStream& stream, const PingreqPacket& packet);

} // end namespace QMQTT

#endif // QMQTT_PINGREQ_PACKET_H
