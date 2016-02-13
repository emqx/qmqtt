#include "basepackettest.h"
#include <qmqtt_pubackpacket.h>
#include <gtest/gtest.h>

using namespace testing;

class PubackPacketTest : public Test
{
public:
    PubackPacketTest() {}
    virtual ~PubackPacketTest() {}

    QMQTT::PubackPacket _packet;
};

TEST_F(PubackPacketTest, defaultConstructorValues_Test)
{
    EXPECT_EQ(QMQTT::PubackType, _packet.type());
    EXPECT_EQ(0, _packet.packetIdentifier());
}

TEST_F(PubackPacketTest, setPacketIdentifier_Test)
{
    _packet.setPacketIdentifier(42);
    EXPECT_EQ(42, _packet.packetIdentifier());
}

class PubackPacketTestWithStream : public BasePacketTest
{
public:
    PubackPacketTestWithStream() {}
    virtual ~PubackPacketTestWithStream() {}

    QMQTT::PubackPacket _packet;

    void streamIntoPacket(
        const quint8 fixedHeader,
        const qint64 remainingLength,
        const quint16 packetIdentifier)
    {
        _stream << fixedHeader;
        writeRemainingLength(remainingLength);
        _stream << packetIdentifier;
        _buffer.seek(0);
        _stream >> _packet;
    }
};

TEST_F(PubackPacketTestWithStream, fixedHeaderTypeWritesConnectTypeToStream_Test)
{
    _stream << _packet;

    EXPECT_EQ(QMQTT::PubackType,
              static_cast<QMQTT::PacketType>((readUInt8(0) & 0xf0) >> 4));
}

TEST_F(PubackPacketTestWithStream, packetIdentifierWritesToStream_Test)
{
    _packet.setPacketIdentifier(42);
    _stream << _packet;

    EXPECT_EQ(42, readUInt16(variableHeaderOffset()));
}

TEST_F(PubackPacketTestWithStream, fixedHeaderFlagsReadsPacketIdentifierFromStream_Test)
{
    streamIntoPacket(0x40, 18, 42);

    EXPECT_EQ(42, _packet.packetIdentifier());
}
