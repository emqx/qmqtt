#ifndef QMQTT_DISCONNECT_PACKET_H
#define QMQTT_DISCONNECT_PACKET_H

#include "qmqtt_abstractpacket.h"
#include <QDataStream>

namespace QMQTT
{

class DisconnectPacket : public AbstractPacket
{
public:
    DisconnectPacket();
    virtual ~DisconnectPacket();

    bool isValid() const;
    PacketType type() const;

protected:
    qint64 calculateRemainingLengthFromData() const;

private:
    friend QDataStream& operator>>(QDataStream& stream, DisconnectPacket& packet);
    friend QDataStream& operator<<(QDataStream& stream, const DisconnectPacket& packet);
};

QDataStream& operator>>(QDataStream& stream, DisconnectPacket& packet);
QDataStream& operator<<(QDataStream& stream, const DisconnectPacket& packet);

} // end namespace QMQTT

#endif // QMQTT_DISCONNECT_PACKET_H
