#ifndef BASE_PACKET_TEST_H
#define BASE_PACKET_TEST_H

#include <QByteArray>
#include <QBuffer>
#include <QDataStream>
#include <gtest/gtest.h>

class BasePacketTest : public testing::Test
{
public:
    BasePacketTest();
    virtual ~BasePacketTest();

    QByteArray _byteArray;
    QBuffer _buffer;
    QDataStream _stream;

    quint8 readUInt8(int offset);
    quint16 readUInt16(int offset);
    QString readString(int offset);
    void writeString(const QString& string);
    void writeRemainingLength(qint64 remainingLength);
    qint64 variableHeaderOffset();
};

#endif // BASE_PACKET_TEST_H

