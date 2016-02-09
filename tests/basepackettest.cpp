#include "basepackettest.h"

using namespace testing;

BasePacketTest::BasePacketTest()
{
    _buffer.setBuffer(&_byteArray);
    _buffer.open(QIODevice::ReadWrite);
    _stream.setDevice(&_buffer);
}

BasePacketTest::~BasePacketTest()
{
}

quint8 BasePacketTest::readUInt8(int offset)
{
    _buffer.seek(offset);

    quint8 i = 0;
    _stream >> i;
    return i;
}

quint16 BasePacketTest::readUInt16(int offset)
{
    _buffer.seek(offset);

    quint16 i = 0;
    _stream >> i;
    return i;
}

QString BasePacketTest::readString(int offset)
{
    _buffer.seek(offset);

    quint16 length = 0;
    _stream >> length;

    QByteArray array;
    array.resize(length);
    _stream.readRawData(array.data(), length);

    return QString::fromUtf8(array);
}

void BasePacketTest::writeString(const QString& string)
{
    _stream << static_cast<quint16>(string.size());
    _stream.writeRawData(string.toUtf8().constData(), string.size());
}

void BasePacketTest::writeRemainingLength(qint64 remainingLength)
{
    quint8 encodedByte = 0;
    do {
        encodedByte = remainingLength % 128;
        remainingLength /= 128;
        if (remainingLength > 0)
        {
            encodedByte |= 128;
        }
        _stream << encodedByte;
    } while (remainingLength > 0);
}

qint64 BasePacketTest::variableHeaderOffset()
{
    _buffer.seek(1);

    quint8 i = 0;
    _stream >> i;
    while ((i & 128) != 0)
    {
        _stream >> i;
    }
    return _buffer.pos();
}
