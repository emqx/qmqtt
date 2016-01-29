#include "qmqtt_connectpacket.h"

const QString SUPPORTED_PROTOCOL = QStringLiteral("MQTT");
const quint8 SUPPORTED_PROTOCOL_LEVEL = 0x04;

const quint8 NO_MASK = 0x00;
const quint8 CLEAN_SESSION_MASK = 0x02;
const quint8 WILL_FLAG_MASK = 0x04;
const quint8 WILL_QOS_MASK = 0x18;
const quint8 WILL_RETAIN_MASK = 0x20;
const quint8 PASSWORD_MASK = 0x40;
const quint8 USERNAME_MASK = 0x80;
const QString PROTOCOL = "MQTT";
const quint8 PROTOCOL_LEVEL = 0x04;

QMQTT::ConnectPacket::ConnectPacket()
    : _protocol(SUPPORTED_PROTOCOL)
    , _protocolLevel(PROTOCOL_LEVEL)
    , _connectFlags(0)
    , _cleanSession(false)
    , _willQos(0)
    , _willRetain(false)
    , _keepAlive(300)
{
}

QMQTT::ConnectPacket::~ConnectPacket()
{
}

QString QMQTT::ConnectPacket::protocol() const
{
    return _protocol;
}

void QMQTT::ConnectPacket::setProtocol(const QString& protocol)
{
    _protocol = protocol;
}

quint8 QMQTT::ConnectPacket::protocolLevel() const
{
    return _protocolLevel;
}

void QMQTT::ConnectPacket::setProtocolLevel(const quint8 protocolLevel)
{
    _protocolLevel = protocolLevel;
}

bool QMQTT::ConnectPacket::cleanSession() const
{
    return _cleanSession;
}

void QMQTT::ConnectPacket::setCleanSession(const bool cleanSession)
{
    _cleanSession = cleanSession;
}

QString QMQTT::ConnectPacket::willTopic() const
{
    return _willTopic;
}

void QMQTT::ConnectPacket::setWillTopic(const QString& willTopic)
{
    _willTopic = willTopic;
}

QString QMQTT::ConnectPacket::willMessage() const
{
    return _willMessage;
}

void QMQTT::ConnectPacket::setWillMessage(const QString& willMessage)
{
    _willMessage = willMessage;
}

quint8 QMQTT::ConnectPacket::willQos() const
{
    return _willQos;
}

void QMQTT::ConnectPacket::setWillQos(const quint8 willQos)
{
    _willQos = willQos;
}

bool QMQTT::ConnectPacket::willRetain() const
{
    return _willRetain;
}

void QMQTT::ConnectPacket::setWillRetain(const bool willRetain)
{
    _willRetain = willRetain;
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
}

QString QMQTT::ConnectPacket::password() const
{
    return _password;
}

void QMQTT::ConnectPacket::setPassword(const QString& password)
{
    _password = password;
}

bool QMQTT::ConnectPacket::isValid() const
{
    if (_protocol != SUPPORTED_PROTOCOL)
    {
        return false;
    }

    if (_protocolLevel != SUPPORTED_PROTOCOL_LEVEL)
    {
        return false;
    }

    if (_connectFlags)
    {
    }

    QRegExp re("[\\da-zA-Z]{1,23}");
    if (!re.exactMatch(_clientIdentifier))
    {
        return false;
    }

    if (_willTopic.isEmpty())
    {
        if (!_willMessage.isEmpty())
        {
            return false;
        }

        if (_willQos != 0)
        {
            return false;
        }

        if(_willRetain == true)
        {
            return false;
        }
    }

    if (_userName.isEmpty())
    {
        if (!_password.isEmpty())
        {
            return false;
        }
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

QDataStream& QMQTT::operator>>(QDataStream& stream, ConnectPacket& packet)
{
    packet._protocol = readStringWith16BitHeader(stream);

    stream >> packet._protocolLevel;

    quint8 connectFlags = 0;
    stream >> connectFlags;

    packet._cleanSession = ((connectFlags & CLEAN_SESSION_MASK) == CLEAN_SESSION_MASK);

    stream >> packet._keepAlive;

    packet._clientIdentifier = readStringWith16BitHeader(stream);

    packet._willTopic.clear();
    packet._willMessage.clear();
    packet._willQos = 0;
    packet._willRetain = false;
    if ((connectFlags & WILL_FLAG_MASK) == WILL_FLAG_MASK)
    {
        packet._willTopic = readStringWith16BitHeader(stream);
        packet._willMessage = readStringWith16BitHeader(stream);
        packet._willQos = ((connectFlags & WILL_QOS_MASK) >> 3);
        packet._willRetain = ((connectFlags & WILL_RETAIN_MASK) == WILL_RETAIN_MASK);
    }

    packet._userName.clear();
    if ((connectFlags & USERNAME_MASK) == USERNAME_MASK)
    {
        packet._userName = readStringWith16BitHeader(stream);
    }

    packet._password.clear();
    if ((connectFlags & PASSWORD_MASK) == PASSWORD_MASK)
    {
        packet._password = readStringWith16BitHeader(stream);
    }

    return stream;
}

void QMQTT::writeStringWith16BitHeader(QDataStream& stream, const QString& string)
{
    stream << static_cast<quint16>(string.size());
    stream.writeRawData(string.toUtf8().constData(), string.size());
}

QDataStream& QMQTT::operator<<(QDataStream& stream, const ConnectPacket& packet)
{
    writeStringWith16BitHeader(stream, packet._protocol);

    stream << packet._protocolLevel;

    quint8 connectFlags = 0;
    connectFlags |= (packet._cleanSession ? CLEAN_SESSION_MASK : NO_MASK);
    if (!packet._willTopic.isEmpty())
    {
        connectFlags |= WILL_FLAG_MASK;
        connectFlags |= ((packet._willQos * 8) & WILL_QOS_MASK);
        connectFlags |= (packet._willRetain ? WILL_RETAIN_MASK : NO_MASK);
    }
    connectFlags |= (packet._userName.isEmpty() ? NO_MASK : USERNAME_MASK);
    connectFlags |= (packet._password.isEmpty() ? NO_MASK : PASSWORD_MASK);
    stream << connectFlags;

    stream << packet._keepAlive;

    writeStringWith16BitHeader(stream, packet._clientIdentifier);

    if (!packet._willTopic.isEmpty())
    {
        writeStringWith16BitHeader(stream, packet._willTopic);
        writeStringWith16BitHeader(stream, packet._willMessage);
    }

    if (!packet._userName.isEmpty())
    {
        writeStringWith16BitHeader(stream, packet._userName);
    }

    if (!packet._password.isEmpty())
    {
        writeStringWith16BitHeader(stream, packet._password);
    }

    return stream;
}
