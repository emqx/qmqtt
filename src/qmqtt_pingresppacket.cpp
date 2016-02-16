#include "qmqtt_pingresppacket.h"
#include <QDataStream>

const quint8 DEFAULT_FIXED_HEADER = QMQTT::PingrespType << 4;

QMQTT::PingrespPacket::PingrespPacket()
    : AbstractPacket(DEFAULT_FIXED_HEADER)
{
}

QMQTT::PingrespPacket::~PingrespPacket()
{
}

QMQTT::PacketType QMQTT::PingrespPacket::type() const
{
    return QMQTT::PingrespType;
}

qint64 QMQTT::PingrespPacket::calculateRemainingLengthFromData() const
{
    return 0;
}

bool QMQTT::PingrespPacket::isValid() const
{
    if (_fixedHeader != DEFAULT_FIXED_HEADER)
    {
        return false;
    }

    return true;
}

QDataStream& QMQTT::operator>>(QDataStream& stream, PingrespPacket& packet)
{
    stream >> packet._fixedHeader;
    packet.readRemainingLength(stream);

    return stream;
}

QDataStream& QMQTT::operator<<(QDataStream& stream, const PingrespPacket& packet)
{
    stream << packet._fixedHeader;
    packet.writeRemainingLength(stream);

    return stream;
}
