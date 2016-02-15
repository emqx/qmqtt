#include "qmqtt_connackpacket.h"

const quint8 DEFAULT_FIXED_HEADER = QMQTT::ConnackType << 4;
const quint8 NO_MASK = 0x00;
const quint8 RESERVED_MASK = 0xfe;
const quint8 SESSION_PRESENT_MASK = 0x01;

QMQTT::ConnackPacket::ConnackPacket()
    : AbstractPacket(DEFAULT_FIXED_HEADER)
    , _connectAcknowledgeFlags(NO_MASK)
    , _connectReturnCode(ConnectionAccepted)
{
}

QMQTT::ConnackPacket::~ConnackPacket()
{
}

QMQTT::PacketType QMQTT::ConnackPacket::type() const
{
    return QMQTT::ConnackType;
}

qint64 QMQTT::ConnackPacket::calculateRemainingLengthFromData() const
{
    return 2;
}

bool QMQTT::ConnackPacket::sessionPresent() const
{
    return (_connectAcknowledgeFlags & SESSION_PRESENT_MASK) != 0;
}

void QMQTT::ConnackPacket::setSessionPresent(const bool sessionPresent)
{
    _connectAcknowledgeFlags &= ~SESSION_PRESENT_MASK;
    if (sessionPresent)
    {
        _connectAcknowledgeFlags |= SESSION_PRESENT_MASK;
    }
}

QMQTT::ConnackPacket::ConnectReturnCodeType QMQTT::ConnackPacket::connectReturnCode() const
{
    return _connectReturnCode;
}

void QMQTT::ConnackPacket::setConnectReturnCode(const ConnectReturnCodeType connectReturnCode)
{
    _connectReturnCode = connectReturnCode;
}

bool QMQTT::ConnackPacket::isValid() const
{
    if (_fixedHeader != DEFAULT_FIXED_HEADER)
    {
        return false;
    }

    if ((_connectAcknowledgeFlags & RESERVED_MASK) != 0)
    {
        return false;
    }

    if (!sessionPresent())
    {
        if (connectReturnCode() != ConnectionAccepted)
        {
            return false;
        }
    }

    return true;
}

QDataStream& QMQTT::operator>>(QDataStream& stream, ConnackPacket& packet)
{
    stream >> packet._fixedHeader;
    packet.readRemainingLength(stream);
    stream >> packet._connectAcknowledgeFlags;
    quint8 i = 0;
    stream >> i;
    packet._connectReturnCode = static_cast<QMQTT::ConnackPacket::ConnectReturnCodeType>(i);

    return stream;
}

QDataStream& QMQTT::operator<<(QDataStream& stream, const ConnackPacket& packet)
{
    stream << packet._fixedHeader;
    packet.writeRemainingLength(stream);
    stream << packet._connectAcknowledgeFlags;
    stream << static_cast<quint8>(packet._connectReturnCode);

    return stream;
}
