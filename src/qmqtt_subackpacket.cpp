#include "qmqtt_subackpacket.h"
#include <QDataStream>

const quint8 DEFAULT_FIXED_HEADER = QMQTT::SubackType << 4;

QMQTT::SubackPacket::SubackPacket()
    : AbstractPacket(DEFAULT_FIXED_HEADER)
    , _packetIdentifier(0)
    , _returnCode(SubackSuccessMaximumQoS0)
{
}

QMQTT::SubackPacket::~SubackPacket()
{
}

QMQTT::PacketType QMQTT::SubackPacket::type() const
{
    return QMQTT::SubackType;
}

qint64 QMQTT::SubackPacket::calculateRemainingLengthFromData() const
{
    return 2;
}

bool QMQTT::SubackPacket::isValid() const
{
    if (_fixedHeader != DEFAULT_FIXED_HEADER)
    {
        return false;
    }

    if (_returnCode > 0x02 && _returnCode != 0x80)
    {
        return false;
    }

    return true;
}

quint16 QMQTT::SubackPacket::packetIdentifier() const
{
    return _packetIdentifier;
}

void QMQTT::SubackPacket::setPacketIdentifier(const quint16 packetIdentifier)
{
    _packetIdentifier = packetIdentifier;
}

QMQTT::SubackReturnCodeType QMQTT::SubackPacket::returnCode() const
{
    return _returnCode;
}

void QMQTT::SubackPacket::setReturnCode(const SubackReturnCodeType returnCode)
{
    _returnCode = returnCode;
}

QDataStream& QMQTT::operator>>(QDataStream& stream, SubackPacket& packet)
{
    stream >> packet._fixedHeader;
    packet.readRemainingLength(stream);
    stream >> packet._packetIdentifier;
    quint8 i = 0;
    stream >> i;
    packet._returnCode = static_cast<SubackReturnCodeType>(i);

    return stream;
}

QDataStream& QMQTT::operator<<(QDataStream& stream, const SubackPacket& packet)
{
    stream << packet._fixedHeader;
    packet.writeRemainingLength(stream);
    stream << packet._packetIdentifier;
    stream << static_cast<quint8>(packet._returnCode);

    return stream;
}
