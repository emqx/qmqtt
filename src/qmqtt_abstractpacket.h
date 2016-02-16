#ifndef QMQTT_ABSTRACT_PACKET_H
#define QMQTT_ABSTRACT_PACKET_H

#include <qglobal.h>

namespace QMQTT
{

enum PacketType
{
    ConnectType     = 0x01,
    ConnackType     = 0x02,
    PublishType     = 0x03,
    PubackType      = 0x04,
    PubrecType      = 0x05,
    PubrelType      = 0x06,
    PubcompType     = 0x07,
    SubscribeType   = 0x08,
    SubackType      = 0x09,
    UnsubscribeType = 0x0a,
    UnsubackType    = 0x0b,
    PingreqType     = 0x0c,
    PingrespType    = 0x0d,
    DisconnectType  = 0x0e
};

class AbstractPacket
{
public:
    AbstractPacket(const quint8 fixedHeader);
    virtual ~AbstractPacket();

    virtual bool isValid() const = 0;
    virtual PacketType type() const = 0;
    virtual qint64 calculateRemainingLengthFromData() const = 0;

protected:
    qint64 readRemainingLength(QDataStream& stream) const;
    void writeRemainingLength(QDataStream& stream) const;
    QString readString(QDataStream& stream);
    void writeString(QDataStream& stream, const QString& string) const;

    quint8 _fixedHeader;
};

} // end namespace QMQTT

#endif // QMQTT_ABSTRACT_PACKET_H
