#include "qmqtt_pingreqpacket.h"
#include <QDataStream>

const quint8 DEFAULT_FIXED_HEADER = QMQTT::PingreqType << 4;

QMQTT::PingreqPacket::PingreqPacket()
    : AbstractPacket(DEFAULT_FIXED_HEADER)
{
}

QMQTT::PingreqPacket::~PingreqPacket()
{
}

QMQTT::PacketType QMQTT::PingreqPacket::type() const
{
    return QMQTT::PingreqType;
}

qint64 QMQTT::PingreqPacket::calculateRemainingLengthFromData() const
{
    return 0;
}

bool QMQTT::PingreqPacket::isValid() const
{
    if (_fixedHeader != DEFAULT_FIXED_HEADER)
    {
        return false;
    }

    return true;
}

QDataStream& QMQTT::operator>>(QDataStream& stream, PingreqPacket& packet)
{
    stream >> packet._fixedHeader;
    packet.readRemainingLength(stream);

    return stream;
}

QDataStream& QMQTT::operator<<(QDataStream& stream, const PingreqPacket& packet)
{
    stream << packet._fixedHeader;
    packet.writeRemainingLength(stream);

    return stream;
}
