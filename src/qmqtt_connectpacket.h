#ifndef QMQTT_CONNECT_PACKET_H
#define QMQTT_CONNECT_PACKET_H

#include "qmqtt_abstractpacket.h"
#include <QDataStream>
#include <QString>

namespace QMQTT
{

class ConnectPacket : public AbstractPacket
{
    friend QDataStream& operator>>(QDataStream& stream, ConnectPacket& packet);
    friend QDataStream& operator<<(QDataStream& stream, const ConnectPacket& packet);

public:
    ConnectPacket();
    virtual ~ConnectPacket();

    QString protocol() const;
    quint8 protocolLevel() const;

    bool cleanSession() const;
    QString willTopic() const;
    QString willMessage() const;
    quint8 willQos() const;
    bool willRetain() const;
    QString clientIdentifier() const;
    QString userName() const;
    QString password() const;

    void setCleanSession(const bool cleanSession);
    void setWillTopic(const QString& willTopic);
    void setWillMessage(const QString& willMessage);
    void setWillQos(const quint8 willQos);
    void setWillRetain(const bool willRetain);
    void setClientIdentifier(const QString& clientIdentifier);
    void setUserName(const QString& userName);
    void setPassword(const QString& password);

    bool isValid() const;
    PacketType type() const;

protected:
    QString _protocol;
    quint8 _protocolLevel;
    quint8 _connectFlags;
    QString _willTopic;
    QString _willMessage;
    quint16 _keepAlive;
    QString _clientIdentifier;
    QString _userName;
    QString _password;

    bool willFlag() const;
    bool userNameFlag() const;
    bool passwordFlag() const;
    qint64 calculateRemainingLengthFromData() const;
};

QDataStream& operator>>(QDataStream& stream, ConnectPacket& packet);
QDataStream& operator<<(QDataStream& stream, const ConnectPacket& packet);

} // end namespace QMQTT

#endif // QMQTT_CONNECT_PACKET_H
