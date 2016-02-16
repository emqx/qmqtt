#include "qmqtt_disconnectpacket.h"
#include <QDataStream>

const quint8 DEFAULT_FIXED_HEADER = QMQTT::DisconnectType << 4;

QMQTT::DisconnectPacket::DisconnectPacket()
    : AbstractPacket(DEFAULT_FIXED_HEADER)
{
}

QMQTT::DisconnectPacket::~DisconnectPacket()
{
}

QMQTT::PacketType QMQTT::DisconnectPacket::type() const
{
    return QMQTT::DisconnectType;
}

qint64 QMQTT::DisconnectPacket::calculateRemainingLengthFromData() const
{
    return 0;
}

bool QMQTT::DisconnectPacket::isValid() const
{
    if (_fixedHeader != DEFAULT_FIXED_HEADER)
    {
        return false;
    }

    return true;
}

QDataStream& QMQTT::operator>>(QDataStream& stream, DisconnectPacket& packet)
{
    stream >> packet._fixedHeader;
    packet.readRemainingLength(stream);

    return stream;
}

QDataStream& QMQTT::operator<<(QDataStream& stream, const DisconnectPacket& packet)
{
    stream << packet._fixedHeader;
    packet.writeRemainingLength(stream);

    return stream;
}
