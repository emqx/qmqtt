#include "qmqtt_pubrelpacket.h"
#include <QDataStream>

const quint8 DEFAULT_FIXED_HEADER = (QMQTT::PubrelType << 4) | 0x02;

QMQTT::PubrelPacket::PubrelPacket()
    : AbstractPacket(DEFAULT_FIXED_HEADER)
    , _packetIdentifier(0)
{
}

QMQTT::PubrelPacket::~PubrelPacket()
{
}

QMQTT::PacketType QMQTT::PubrelPacket::type() const
{
    return QMQTT::PubrelType;
}

qint64 QMQTT::PubrelPacket::calculateRemainingLengthFromData() const
{
    return 2;
}

bool QMQTT::PubrelPacket::isValid() const
{
    if (_fixedHeader != DEFAULT_FIXED_HEADER)
    {
        return false;
    }

    return true;
}

quint16 QMQTT::PubrelPacket::packetIdentifier() const
{
    return _packetIdentifier;
}

void QMQTT::PubrelPacket::setPacketIdentifier(const quint16 packetIdentifier)
{
    _packetIdentifier = packetIdentifier;
}

QDataStream& QMQTT::operator>>(QDataStream& stream, PubrelPacket& packet)
{
    stream >> packet._fixedHeader;
    packet.readRemainingLength(stream);
    stream >> packet._packetIdentifier;

    return stream;
}

QDataStream& QMQTT::operator<<(QDataStream& stream, const PubrelPacket& packet)
{
    stream << packet._fixedHeader;
    packet.writeRemainingLength(stream);
    stream << packet._packetIdentifier;

    return stream;
}
