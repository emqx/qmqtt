#include "qmqtt_pubcomppacket.h"
#include <QDataStream>

const quint8 DEFAULT_FIXED_HEADER = QMQTT::PubcompType << 4;

QMQTT::PubcompPacket::PubcompPacket()
    : AbstractPacket(DEFAULT_FIXED_HEADER)
    , _packetIdentifier(0)
{
}

QMQTT::PubcompPacket::~PubcompPacket()
{
}

QMQTT::PacketType QMQTT::PubcompPacket::type() const
{
    return QMQTT::PubcompType;
}

qint64 QMQTT::PubcompPacket::calculateRemainingLengthFromData() const
{
    return 2;
}

bool QMQTT::PubcompPacket::isValid() const
{
    if (_fixedHeader != DEFAULT_FIXED_HEADER)
    {
        return false;
    }

    return true;
}

quint16 QMQTT::PubcompPacket::packetIdentifier() const
{
    return _packetIdentifier;
}

void QMQTT::PubcompPacket::setPacketIdentifier(const quint16 packetIdentifier)
{
    _packetIdentifier = packetIdentifier;
}

QDataStream& QMQTT::operator>>(QDataStream& stream, PubcompPacket& packet)
{
    stream >> packet._fixedHeader;
    packet.readRemainingLength(stream);
    stream >> packet._packetIdentifier;

    return stream;
}

QDataStream& QMQTT::operator<<(QDataStream& stream, const PubcompPacket& packet)
{
    stream << packet._fixedHeader;
    packet.writeRemainingLength(stream);
    stream << packet._packetIdentifier;

    return stream;
}
