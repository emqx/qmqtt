#include "qmqtt_connectpacket.h"
#include <qmath.h>

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
    : _protocol(SUPPORTED_PROTOCOL)
    , _protocolLevel(SUPPORTED_PROTOCOL_LEVEL)
    , _connectFlags(NO_MASK)
    , _keepAlive(DEFAULT_KEEP_ALIVE_INTERVAL)
{
}

QMQTT::ConnectPacket::~ConnectPacket()
{
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

void QMQTT::ConnectPacket::setWillFlag(const bool willFlag)
{
    _connectFlags &= ~WILL_FLAG_MASK;
    if (willFlag)
    {
        _connectFlags |= WILL_FLAG_MASK;
    }
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

void QMQTT::ConnectPacket::setPasswordFlag(const bool passwordFlag)
{
    _connectFlags &= ~PASSWORD_MASK;
    if (passwordFlag)
    {
        _connectFlags |= PASSWORD_MASK;
    }
}

bool QMQTT::ConnectPacket::userNameFlag() const
{
    return (_connectFlags & USERNAME_MASK) != 0;
}

void QMQTT::ConnectPacket::setUserNameFlag(const bool userNameFlag)
{
    _connectFlags &= ~USERNAME_MASK;
    if (userNameFlag)
    {
        _connectFlags |= USERNAME_MASK;
    }
}

QString QMQTT::ConnectPacket::willTopic() const
{
    return _willTopic;
}

void QMQTT::ConnectPacket::setWillTopic(const QString& willTopic)
{
    _willTopic = willTopic;
    setWillFlag(!_willTopic.isEmpty());
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
    setUserNameFlag(!_userName.isEmpty());
}

QString QMQTT::ConnectPacket::password() const
{
    return _password;
}

void QMQTT::ConnectPacket::setPassword(const QString& password)
{
    _password = password;
    setPasswordFlag(!_password.isEmpty());
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
        if (  !willTopic().isEmpty() || !willMessage().isEmpty()
            || willQos() != 0        || willRetain())
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

QString QMQTT::readStringWith16BitHeader(QDataStream& stream)
{
    quint16 length;
    stream >> length;
    QByteArray byteArray(length, '\0');
    stream.readRawData(byteArray.data(), length);
    return QString::fromUtf8(byteArray);
}

void QMQTT::writeStringWith16BitHeader(QDataStream& stream, const QString& string)
{
    stream << static_cast<quint16>(string.size());
    stream.writeRawData(string.toUtf8().constData(), string.size());
}

QDataStream& QMQTT::operator>>(QDataStream& stream, ConnectPacket& packet)
{
    packet._protocol = readStringWith16BitHeader(stream);
    stream >> packet._protocolLevel;
    stream >> packet._connectFlags;
    stream >> packet._keepAlive;

    // payload
    packet._clientIdentifier = readStringWith16BitHeader(stream);
    packet._willTopic.clear();
    packet._willMessage.clear();
    if (packet.willFlag())
    {
        packet._willTopic = readStringWith16BitHeader(stream);
        packet._willMessage = readStringWith16BitHeader(stream);
    }
    packet._userName.clear();
    if (packet.userNameFlag())
    {
        packet._userName = readStringWith16BitHeader(stream);
    }
    packet._password.clear();
    if (packet.passwordFlag())
    {
        packet._password = readStringWith16BitHeader(stream);
    }

    return stream;
}

QDataStream& QMQTT::operator<<(QDataStream& stream, const ConnectPacket& packet)
{
    writeStringWith16BitHeader(stream, packet._protocol);
    stream << packet._protocolLevel;
    stream << packet._connectFlags;
    stream << packet._keepAlive;

    // payload
    writeStringWith16BitHeader(stream, packet._clientIdentifier);
    if (packet.willFlag())
    {
        writeStringWith16BitHeader(stream, packet._willTopic);
        writeStringWith16BitHeader(stream, packet._willMessage);
    }
    if (packet.userNameFlag())
    {
        writeStringWith16BitHeader(stream, packet._userName);
    }
    if (packet.passwordFlag())
    {
        writeStringWith16BitHeader(stream, packet._password);
    }

    return stream;
}
