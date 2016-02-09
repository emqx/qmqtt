#include "qmqtt_abstractpacket.h"
#include <QDataStream>

QMQTT::AbstractPacket::AbstractPacket(const quint8 fixedHeader)
    : _fixedHeader(fixedHeader)
{
}

QMQTT::AbstractPacket::~AbstractPacket()
{
}

qint64 QMQTT::AbstractPacket::readRemainingLength(QDataStream& stream) const
{
    int multiplier = 1;
    qint64 remainingLength = 0;
    quint8 encodedByte = 0;
    do {
        stream >> encodedByte;
        remainingLength += (encodedByte & 127) * multiplier;
        multiplier *= 128;
    } while ((encodedByte & 128) != 0);
    return remainingLength;
}

void QMQTT::AbstractPacket::writeRemainingLength(QDataStream& stream) const
{
    qint64 remainingLength = calculateRemainingLengthFromData();
    quint8 encodedByte = 0;
    do {
        encodedByte = remainingLength % 128;
        remainingLength /= 128;
        if (remainingLength > 0)
        {
            encodedByte |= 128;
        }
        stream << encodedByte;
    } while (remainingLength > 0);
}

QString QMQTT::AbstractPacket::readString(QDataStream& stream)
{
    quint16 length;
    stream >> length;
    QByteArray byteArray;
    byteArray.resize(length);
    stream.readRawData(byteArray.data(), length);
    return QString::fromUtf8(byteArray);
}

void QMQTT::AbstractPacket::writeString(QDataStream& stream, const QString& string) const
{
    stream << static_cast<quint16>(string.size());
    stream.writeRawData(string.toUtf8().constData(), string.size());
}
