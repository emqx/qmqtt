#include "qmqtt_pubrecpacket.h"
#include <QDataStream>

const quint8 DEFAULT_FIXED_HEADER = QMQTT::PubrecType << 4;

QMQTT::PubrecPacket::PubrecPacket()
    : AbstractPacket(DEFAULT_FIXED_HEADER)
    , _packetIdentifier(0)
{
}

QMQTT::PubrecPacket::~PubrecPacket()
{
}

QMQTT::PacketType QMQTT::PubrecPacket::type() const
{
    return QMQTT::PubrecType;
}

qint64 QMQTT::PubrecPacket::calculateRemainingLengthFromData() const
{
    return 2;
}

bool QMQTT::PubrecPacket::isValid() const
{
    return true;
}

quint16 QMQTT::PubrecPacket::packetIdentifier() const
{
    return _packetIdentifier;
}

void QMQTT::PubrecPacket::setPacketIdentifier(const quint16 packetIdentifier)
{
    _packetIdentifier = packetIdentifier;
}

QDataStream& QMQTT::operator>>(QDataStream& stream, PubrecPacket& packet)
{
    stream >> packet._fixedHeader;
    packet.readRemainingLength(stream);
    stream >> packet._packetIdentifier;

    return stream;
}

QDataStream& QMQTT::operator<<(QDataStream& stream, const PubrecPacket& packet)
{
    stream << packet._fixedHeader;
    packet.writeRemainingLength(stream);
    stream << packet._packetIdentifier;

    return stream;
}
