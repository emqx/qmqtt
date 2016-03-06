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

    quint8 readUInt8(int offset = -1);
    quint16 readUInt16(int offset = -1);
    QString readString(int offset = -1);

    void writeString(const QString& string);
    qint64 readRemainingLength();
    void writeRemainingLength(qint64 remainingLength);
    qint64 variableHeaderOffset();
    QByteArray readByteArray(int offset, int length);
    QByteArray readByteArray(int length);
};

#endif // BASE_PACKET_TEST_H

