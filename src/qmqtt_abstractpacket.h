#ifndef QMQTT_ABSTRACT_PACKET_H
#define QMQTT_ABSTRACT_PACKET_H

#include <qglobal.h>

namespace QMQTT
{

enum PacketType
{
    ConnectType = 0x01,
    ConnackType = 0x02
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
