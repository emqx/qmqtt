#ifndef QMQTT_CONNECT_PACKET_H
#define QMQTT_CONNECT_PACKET_H

#include <QDataStream>

namespace QMQTT
{

class ConnectPacket
{
    friend QDataStream& operator>>(QDataStream& stream, QMQTT::ConnectPacket& packet);

public:
    ConnectPacket();
    virtual ~ConnectPacket();

    QString protocol() const;
    quint8 protocolLevel() const;
    bool cleanSession() const;
    bool willFlag() const;
    quint8 willQos() const;
    bool willRetain() const;
    bool passwordFlag() const;
    bool userNameFlag() const;

    void setProtocol(const QString& protocol);
    void setProtocolLevel(const quint8 protocolLevel);
    void setCleanSession(const bool cleanSession);
    void setWillFlag(const bool willFlag);
    void setWillQos(const quint8 willQos);
    void setWillRetain(const bool willRetain);
    void setPasswordFlag(const bool passwordFlag);
    void setUserNameFlag(const bool userNameFlag);

protected:
    QString _protocol;
    quint8 _protocolLevel;
    quint8 _connectFlags;
    quint16 _keepAlive;
};

} // end namespace QMQTT

QString readStringWith16BitHeader(QDataStream& stream)
{
    quint16 length;
    stream >> length;
    QByteArray byteArray(length, '\0');
    stream.readRawData(byteArray.data(), length);
    return QString::fromUtf8(byteArray);
}

QDataStream& operator>>(QDataStream& stream, QMQTT::ConnectPacket& packet)
{
    packet._protocol = readStringWith16BitHeader(stream);
    stream >> packet._protocolLevel;
    stream >> packet._connectFlags;
    stream >> packet._keepAlive;
    return stream;
}

#endif // QMQTT_CONNECT_PACKET_H
