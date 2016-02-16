#ifndef QMQTT_SUBACK_PACKET_H
#define QMQTT_SUBACK_PACKET_H

#include "qmqtt_abstractpacket.h"
#include <QDataStream>

namespace QMQTT
{

enum SubackReturnCodeType
{
    SubackSuccessMaximumQoS0 = 0x00,
    SubackSuccessMaximumQoS1 = 0x01,
    SubackSuccessMaximumQoS2 = 0x02,
    SubackFailure = 0x80
};

class SubackPacket : public AbstractPacket
{
public:
    SubackPacket();
    virtual ~SubackPacket();

    bool isValid() const;
    PacketType type() const;

    quint16 packetIdentifier() const;
    void setPacketIdentifier(const quint16 packetIdentifier);

    SubackReturnCodeType returnCode() const;
    void setReturnCode(const SubackReturnCodeType returnCode);

protected:
    quint16 _packetIdentifier;
    SubackReturnCodeType _returnCode;

    qint64 calculateRemainingLengthFromData() const;

private:
    friend QDataStream& operator>>(QDataStream& stream, SubackPacket& packet);
    friend QDataStream& operator<<(QDataStream& stream, const SubackPacket& packet);
};

QDataStream& operator>>(QDataStream& stream, SubackPacket& packet);
QDataStream& operator<<(QDataStream& stream, const SubackPacket& packet);

} // end namespace QMQTT

#endif // QMQTT_SUBACK_PACKET_H
