#ifndef QMQTT_PINGRESP_PACKET_H
#define QMQTT_PINGRESP_PACKET_H

#include "qmqtt_abstractpacket.h"
#include <QDataStream>

namespace QMQTT
{

class PingrespPacket : public AbstractPacket
{
public:
    PingrespPacket();
    virtual ~PingrespPacket();

    bool isValid() const;
    PacketType type() const;

protected:
    qint64 calculateRemainingLengthFromData() const;

private:
    friend QDataStream& operator>>(QDataStream& stream, PingrespPacket& packet);
    friend QDataStream& operator<<(QDataStream& stream, const PingrespPacket& packet);
};

QDataStream& operator>>(QDataStream& stream, PingrespPacket& packet);
QDataStream& operator<<(QDataStream& stream, const PingrespPacket& packet);

} // end namespace QMQTT

#endif // QMQTT_PINGRESP_PACKET_H
