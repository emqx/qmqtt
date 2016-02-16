#include "basepackettest.h"
#include <qmqtt_unsubackpacket.h>
#include <gtest/gtest.h>

using namespace testing;

class UnsubackPacketTest : public Test
{
public:
    UnsubackPacketTest() {}
    virtual ~UnsubackPacketTest() {}

    QMQTT::UnsubackPacket _packet;
};

TEST_F(UnsubackPacketTest, defaultConstructorValues_Test)
{
    EXPECT_EQ(QMQTT::UnsubackType, _packet.type());
    EXPECT_EQ(0, _packet.packetIdentifier());
}

TEST_F(UnsubackPacketTest, setPacketIdentifier_Test)
{
    _packet.setPacketIdentifier(42);
    EXPECT_EQ(42, _packet.packetIdentifier());
}

class UnsubackPacketTestWithStream : public BasePacketTest
{
public:
    UnsubackPacketTestWithStream() {}
    virtual ~UnsubackPacketTestWithStream() {}

    QMQTT::UnsubackPacket _packet;

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

TEST_F(UnsubackPacketTestWithStream, fixedHeaderTypeWritesUnsubackTypeToStream_Test)
{
    _stream << _packet;

    EXPECT_EQ(QMQTT::UnsubackType,
              static_cast<QMQTT::PacketType>((readUInt8(0) & 0xf0) >> 4));
}

TEST_F(UnsubackPacketTestWithStream, packetIdentifierWritesToStream_Test)
{
    _packet.setPacketIdentifier(42);
    _stream << _packet;

    EXPECT_EQ(42, readUInt16(variableHeaderOffset()));
}

TEST_F(UnsubackPacketTestWithStream, packetIdentifierReadsFromStream_Test)
{
    streamIntoPacket(QMQTT::UnsubackType << 4, 2, 42);

    EXPECT_EQ(42, _packet.packetIdentifier());
}

TEST_F(UnsubackPacketTestWithStream, typeUnsubackAndFixedHeaderFlagsZeroIsValid_Test)
{
    streamIntoPacket(QMQTT::UnsubackType << 4, 2, 42);

    EXPECT_TRUE(_packet.isValid());
}

TEST_F(UnsubackPacketTestWithStream, typeNotUnsubackAndFixedHeaderFlagsZeroIsInvalid_Test)
{
    streamIntoPacket(QMQTT::ConnectType << 4, 2, 42);

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(UnsubackPacketTestWithStream, typeUnsubackAndFixedHeaderFlagsNotZeroIsInvalid_Test)
{
    streamIntoPacket((QMQTT::UnsubackType << 4) | 0x01, 2, 42);

    EXPECT_FALSE(_packet.isValid());
}

