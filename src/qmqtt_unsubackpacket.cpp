#include "qmqtt_unsubackpacket.h"
#include <QDataStream>

const quint8 DEFAULT_FIXED_HEADER = QMQTT::UnsubackType << 4;

QMQTT::UnsubackPacket::UnsubackPacket()
    : AbstractPacket(DEFAULT_FIXED_HEADER)
    , _packetIdentifier(0)
{
}

QMQTT::UnsubackPacket::~UnsubackPacket()
{
}

QMQTT::PacketType QMQTT::UnsubackPacket::type() const
{
    return QMQTT::UnsubackType;
}

qint64 QMQTT::UnsubackPacket::calculateRemainingLengthFromData() const
{
    return 2;
}

bool QMQTT::UnsubackPacket::isValid() const
{
    if (_fixedHeader != DEFAULT_FIXED_HEADER)
    {
        return false;
    }

    return true;
}

quint16 QMQTT::UnsubackPacket::packetIdentifier() const
{
    return _packetIdentifier;
}

void QMQTT::UnsubackPacket::setPacketIdentifier(const quint16 packetIdentifier)
{
    _packetIdentifier = packetIdentifier;
}

QDataStream& QMQTT::operator>>(QDataStream& stream, UnsubackPacket& packet)
{
    stream >> packet._fixedHeader;
    packet.readRemainingLength(stream);
    stream >> packet._packetIdentifier;

    return stream;
}

QDataStream& QMQTT::operator<<(QDataStream& stream, const UnsubackPacket& packet)
{
    stream << packet._fixedHeader;
    packet.writeRemainingLength(stream);
    stream << packet._packetIdentifier;

    return stream;
}
