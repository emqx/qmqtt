#include "qmqtt_connectpacket.h"

quint8 NO_MASK = 0x00;
quint8 CLEAN_SESSION_MASK = 0x02;
quint8 WILL_FLAG_MASK = 0x04;
quint8 WILL_QOS_MASK = 0x18;
quint8 WILL_RETAIN_MASK = 0x20;
quint8 PASSWORD_MASK = 0x40;
quint8 USERNAME_MASK = 0x80;

QMQTT::ConnectPacket::ConnectPacket()
    : _protocolLevel(0)
    , _connectFlags(0)
    , _keepAlive(0)
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
    return (_connectFlags & CLEAN_SESSION_MASK) == CLEAN_SESSION_MASK;
}

void QMQTT::ConnectPacket::setCleanSession(const bool cleanSession)
{
    _connectFlags |= (cleanSession ? CLEAN_SESSION_MASK : NO_MASK);
}

bool QMQTT::ConnectPacket::willFlag() const
{
    return (_connectFlags & WILL_FLAG_MASK) == WILL_FLAG_MASK;
}

void QMQTT::ConnectPacket::setWillFlag(const bool willFlag)
{
    _connectFlags |= (willFlag ? WILL_FLAG_MASK : NO_MASK);
}

quint8 QMQTT::ConnectPacket::willQos() const
{
    return (_connectFlags & WILL_QOS_MASK) / 8;
}

void QMQTT::ConnectPacket::setWillQos(const quint8 willQos)
{
    _connectFlags |= ((willQos * 8) & WILL_QOS_MASK);
}

bool QMQTT::ConnectPacket::willRetain() const
{
    return (_connectFlags & WILL_RETAIN_MASK) == WILL_RETAIN_MASK;
}

void QMQTT::ConnectPacket::setWillRetain(const bool willRetain)
{
    _connectFlags |= (willRetain ? WILL_RETAIN_MASK : NO_MASK);
}

bool QMQTT::ConnectPacket::passwordFlag() const
{
    return (_connectFlags & PASSWORD_MASK) == PASSWORD_MASK;
}

void QMQTT::ConnectPacket::setPasswordFlag(const bool passwordFlag)
{
    _connectFlags |= (passwordFlag ? PASSWORD_MASK : NO_MASK);
}

bool QMQTT::ConnectPacket::userNameFlag() const
{
    return (_connectFlags & USERNAME_MASK) == USERNAME_MASK;
}

void QMQTT::ConnectPacket::setUserNameFlag(const bool userNameFlag)
{
    _connectFlags |= (userNameFlag ? USERNAME_MASK : NO_MASK);
}

