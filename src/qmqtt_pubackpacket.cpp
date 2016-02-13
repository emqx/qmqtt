#include "qmqtt_pubackpacket.h"
#include <QDataStream>

const quint8 DEFAULT_FIXED_HEADER = QMQTT::PubackType << 4;

QMQTT::PubackPacket::PubackPacket()
    : AbstractPacket(DEFAULT_FIXED_HEADER)
    , _packetIdentifier(0)
{
}

QMQTT::PubackPacket::~PubackPacket()
{
}

QMQTT::PacketType QMQTT::PubackPacket::type() const
{
    return QMQTT::PubackType;
}

qint64 QMQTT::PubackPacket::calculateRemainingLengthFromData() const
{
    return 2;
}

bool QMQTT::PubackPacket::isValid() const
{
    return true;
}

quint16 QMQTT::PubackPacket::packetIdentifier() const
{
    return _packetIdentifier;
}

void QMQTT::PubackPacket::setPacketIdentifier(const quint16 packetIdentifier)
{
    _packetIdentifier = packetIdentifier;
}

QDataStream& QMQTT::operator>>(QDataStream& stream, PubackPacket& packet)
{
    stream >> packet._fixedHeader;
    packet.readRemainingLength(stream);
    stream >> packet._packetIdentifier;

    return stream;
}

QDataStream& QMQTT::operator<<(QDataStream& stream, const PubackPacket& packet)
{
    stream << packet._fixedHeader;
    packet.writeRemainingLength(stream);
    stream << packet._packetIdentifier;

    return stream;
}
