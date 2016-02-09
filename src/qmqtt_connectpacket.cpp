#include "qmqtt_connectpacket.h"
#include <qmath.h>

const quint8 DEFAULT_FIXED_HEADER = 0x10;
const QString SUPPORTED_PROTOCOL = QStringLiteral("MQTT");
const quint8 SUPPORTED_PROTOCOL_LEVEL = 0x04;
const quint16 DEFAULT_KEEP_ALIVE_INTERVAL = 300;
const quint8 NO_MASK = 0x00;
const quint8 RESERVED_MASK = 0x01;
const quint8 CLEAN_SESSION_MASK = 0x02;
const quint8 WILL_FLAG_MASK = 0x04;
const quint8 WILL_QOS_MASK = 0x18;
const quint8 WILL_RETAIN_MASK = 0x20;
const quint8 PASSWORD_MASK = 0x40;
const quint8 USERNAME_MASK = 0x80;

QMQTT::ConnectPacket::ConnectPacket()
    : AbstractPacket(DEFAULT_FIXED_HEADER)
    , _protocol(SUPPORTED_PROTOCOL)
    , _protocolLevel(SUPPORTED_PROTOCOL_LEVEL)
    , _connectFlags(NO_MASK)
    , _keepAlive(DEFAULT_KEEP_ALIVE_INTERVAL)
{
}

QMQTT::ConnectPacket::~ConnectPacket()
{
}

QMQTT::PacketType QMQTT::ConnectPacket::type() const
{
    return ConnectType;
}

QString QMQTT::ConnectPacket::protocol() const
{
    return _protocol;
}

quint8 QMQTT::ConnectPacket::protocolLevel() const
{
    return _protocolLevel;
}

bool QMQTT::ConnectPacket::cleanSession() const
{
    return (_connectFlags & CLEAN_SESSION_MASK) != 0;
}

void QMQTT::ConnectPacket::setCleanSession(const bool cleanSession)
{
    _connectFlags &= ~CLEAN_SESSION_MASK;
    if (cleanSession)
    {
        _connectFlags |= CLEAN_SESSION_MASK;
    }
}

bool QMQTT::ConnectPacket::willFlag() const
{
    return (_connectFlags & WILL_FLAG_MASK) != 0;
}

quint8 QMQTT::ConnectPacket::willQos() const
{
    return (_connectFlags & WILL_QOS_MASK) >> 3;
}

void QMQTT::ConnectPacket::setWillQos(const quint8 willQos)
{
    _connectFlags &= ~WILL_QOS_MASK;
    _connectFlags |= (qMin(static_cast<quint8>(2), willQos) << 3);
}

bool QMQTT::ConnectPacket::willRetain() const
{
    return (_connectFlags & WILL_RETAIN_MASK) != 0;
}

void QMQTT::ConnectPacket::setWillRetain(const bool willRetain)
{
    _connectFlags &= ~WILL_RETAIN_MASK;
    if (willRetain)
    {
        _connectFlags |= WILL_RETAIN_MASK;
    }
}

bool QMQTT::ConnectPacket::passwordFlag() const
{
    return (_connectFlags & PASSWORD_MASK) != 0;
}

bool QMQTT::ConnectPacket::userNameFlag() const
{
    return (_connectFlags & USERNAME_MASK) != 0;
}

QString QMQTT::ConnectPacket::willTopic() const
{
    return _willTopic;
}

void QMQTT::ConnectPacket::setWillTopic(const QString& willTopic)
{
    _willTopic = willTopic;
    _connectFlags &= ~WILL_FLAG_MASK;
    if (!_willTopic.isEmpty())
    {
        _connectFlags |= WILL_FLAG_MASK;
    }
}

QString QMQTT::ConnectPacket::willMessage() const
{
    return _willMessage;
}

void QMQTT::ConnectPacket::setWillMessage(const QString& willMessage)
{
    _willMessage = willMessage;
}

QString QMQTT::ConnectPacket::clientIdentifier() const
{
    return _clientIdentifier;
}

void QMQTT::ConnectPacket::setClientIdentifier(const QString& clientIdentifier)
{
    _clientIdentifier = clientIdentifier;
}

QString QMQTT::ConnectPacket::userName() const
{
    return _userName;
}

void QMQTT::ConnectPacket::setUserName(const QString& userName)
{
    _userName = userName;
    _connectFlags &= ~USERNAME_MASK;
    if (!_userName.isEmpty())
    {
        _connectFlags |= USERNAME_MASK;
    }
}

QString QMQTT::ConnectPacket::password() const
{
    return _password;
}

void QMQTT::ConnectPacket::setPassword(const QString& password)
{
    _password = password;
    _connectFlags &= ~PASSWORD_MASK;
    if (!_password.isEmpty())
    {
        _connectFlags |= PASSWORD_MASK;
    }

}

bool QMQTT::ConnectPacket::isValid() const
{
    if (protocol() != SUPPORTED_PROTOCOL)
    {
        return false;
    }

    if (protocolLevel() != SUPPORTED_PROTOCOL_LEVEL)
    {
        return false;
    }

    if ((_connectFlags & RESERVED_MASK) != 0)
    {
        return false;
    }

    if (willFlag())
    {
        if (willTopic().isEmpty() || willQos() > 2)
        {
            return false;
        }
    }
    else
    {
        if (willQos() != 0 || willRetain())
        {
            return false;
        }
    }

    if (userNameFlag() == userName().isEmpty())
    {
        return false;
    }

    if (!userNameFlag() && passwordFlag())
    {
        return false;
    }

    if (passwordFlag() == password().isEmpty())
    {
        return false;
    }

    QRegExp re("[\\da-zA-Z]{1,23}");
    if (!re.exactMatch(_clientIdentifier))
    {
        return false;
    }

    return true;
}

qint64 QMQTT::ConnectPacket::calculateRemainingLengthFromData() const
{
    qint64 remainingLength = 12 + _clientIdentifier.size();
    if (willFlag())
    {
        remainingLength += 4 + _willTopic.size() + _willMessage.size();
    }
    if (userNameFlag())
    {
        remainingLength += 2 + _userName.size();
    }
    if (passwordFlag())
    {
        remainingLength += 2 + _password.size();
    }
    return remainingLength;
}

QDataStream& QMQTT::operator>>(QDataStream& stream, ConnectPacket& packet)
{
    stream >> packet._fixedHeader;
    packet.readRemainingLength(stream);

    packet._protocol = packet.readString(stream);
    stream >> packet._protocolLevel;
    stream >> packet._connectFlags;
    stream >> packet._keepAlive;

    packet._clientIdentifier = packet.readString(stream);
    packet._willTopic.clear();
    packet._willMessage.clear();
    if (packet.willFlag())
    {
        packet._willTopic = packet.readString(stream);
        packet._willMessage = packet.readString(stream);
    }
    packet._userName.clear();
    if (packet.userNameFlag())
    {
        packet._userName = packet.readString(stream);
    }
    packet._password.clear();
    if (packet.passwordFlag())
    {
        packet._password = packet.readString(stream);
    }

    return stream;
}

QDataStream& QMQTT::operator<<(QDataStream& stream, const ConnectPacket& packet)
{
    stream << packet._fixedHeader;
    packet.writeRemainingLength(stream);

    packet.writeString(stream, packet._protocol);
    stream << packet._protocolLevel;
    stream << packet._connectFlags;
    stream << packet._keepAlive;

    packet.writeString(stream, packet._clientIdentifier);
    if (packet.willFlag())
    {
        packet.writeString(stream, packet._willTopic);
        packet.writeString(stream, packet._willMessage);
    }
    if (packet.userNameFlag())
    {
        packet.writeString(stream, packet._userName);
    }
    if (packet.passwordFlag())
    {
        packet.writeString(stream, packet._password);
    }

    return stream;
}
