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

TEST_F(PubackPacketTestWithStream, fixedHeaderTypeWritesPubackTypeToStream_Test)
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

TEST_F(PubackPacketTestWithStream, packetIdentifierReadsFromStream_Test)
{
    streamIntoPacket(QMQTT::PubackType << 4, 2, 42);

    EXPECT_EQ(42, _packet.packetIdentifier());
}

TEST_F(PubackPacketTestWithStream, typePubackAndFixedHeaderFlagsZeroIsValid_Test)
{
    streamIntoPacket(QMQTT::PubackType << 4, 2, 42);

    EXPECT_TRUE(_packet.isValid());
}

TEST_F(PubackPacketTestWithStream, typeNotPubackAndFixedHeaderFlagsZeroIsInvalid_Test)
{
    streamIntoPacket(QMQTT::ConnectType << 4, 2, 42);

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(PubackPacketTestWithStream, typePubackAndFixedHeaderFlagsNotZeroIsInvalid_Test)
{
    streamIntoPacket((QMQTT::PubackType << 4) | 0x01, 2, 42);

    EXPECT_FALSE(_packet.isValid());
}
