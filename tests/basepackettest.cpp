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
    if (offset >= 0)
    {
        _buffer.seek(offset);
    }
    quint8 i = 0;
    _stream >> i;
    return i;
}

quint16 BasePacketTest::readUInt16(int offset)
{
    if (offset >= 0)
    {
        _buffer.seek(offset);
    }
    quint16 i = 0;
    _stream >> i;
    return i;
}

QString BasePacketTest::readString(int offset)
{
    if (offset >= 0)
    {
        _buffer.seek(offset);
    }

    quint16 length = 0;
    _stream >> length;

    QByteArray array;
    if (length > 0)
    {
        array.resize(length);
        _stream.readRawData(array.data(), length);
    }

    return QString::fromUtf8(array);
}

QByteArray BasePacketTest::readByteArray(int offset, int length)
{
    _buffer.seek(offset);
    return readByteArray(length);
}

QByteArray BasePacketTest::readByteArray(int length)
{
    QByteArray array;
    if (length > 0)
    {
        array.resize(length);
        _stream.readRawData(array.data(), length);
    }
    return array;
}

void BasePacketTest::writeString(const QString& string)
{
    _stream << static_cast<quint16>(string.size());
    if (string.size() > 0)
    {
        _stream.writeRawData(string.toUtf8().constData(), string.size());
    }
}

qint64 BasePacketTest::readRemainingLength()
{
    _buffer.seek(1);
    int multiplier = 1;
    qint64 remainingLength = 0;
    quint8 encodedByte = 0;
    do {
        _stream >> encodedByte;
        remainingLength += (encodedByte & 127) * multiplier;
        multiplier *= 128;
    } while ((encodedByte & 128) != 0);
    return remainingLength;
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
