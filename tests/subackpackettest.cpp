#include "basepackettest.h"
#include <qmqtt_subackpacket.h>
#include <gtest/gtest.h>

using namespace testing;

class SubackPacketTest : public Test
{
public:
    SubackPacketTest() {}
    virtual ~SubackPacketTest() {}

    QMQTT::SubackPacket _packet;
};

TEST_F(SubackPacketTest, defaultConstructorValues_Test)
{
    EXPECT_EQ(QMQTT::SubackType, _packet.type());
    EXPECT_EQ(0, _packet.packetIdentifier());
}

TEST_F(SubackPacketTest, setPacketIdentifier_Test)
{
    _packet.setPacketIdentifier(42);
    EXPECT_EQ(42, _packet.packetIdentifier());
}

class SubackPacketTestWithStream : public BasePacketTest
{
public:
    SubackPacketTestWithStream() {}
    virtual ~SubackPacketTestWithStream() {}

    QMQTT::SubackPacket _packet;

    void streamIntoPacket(
        const quint8 fixedHeader,
        const qint64 remainingLength,
        const quint16 packetIdentifier,
        const quint8 returnCode)
    {
        _stream << fixedHeader;
        writeRemainingLength(remainingLength);
        _stream << packetIdentifier;
        _stream << returnCode;
        _buffer.seek(0);
        _stream >> _packet;
    }
};

TEST_F(SubackPacketTestWithStream, fixedHeaderTypeWritesSubackTypeToStream_Test)
{
    _stream << _packet;

    EXPECT_EQ(QMQTT::SubackType,
              static_cast<QMQTT::PacketType>((readUInt8(0) & 0xf0) >> 4));
}

TEST_F(SubackPacketTestWithStream, packetIdentifierWritesToStream_Test)
{
    _packet.setPacketIdentifier(42);
    _stream << _packet;

    EXPECT_EQ(42, readUInt16(variableHeaderOffset()));
}

TEST_F(SubackPacketTestWithStream, returnCodeWritesToStream_Test)
{
    _packet.setReturnCode(QMQTT::SubackSuccessMaximumQoS2);
    _stream << _packet;

    EXPECT_EQ(QMQTT::SubackSuccessMaximumQoS2, readUInt8(variableHeaderOffset()+2));
}

TEST_F(SubackPacketTestWithStream, packetIdentifierReadsFromStream_Test)
{
    streamIntoPacket(QMQTT::SubackType << 4, 2, 42, 0x00);

    EXPECT_EQ(42, _packet.packetIdentifier());
}

TEST_F(SubackPacketTestWithStream, returnCodeReadsFromStream_Test)
{
    streamIntoPacket(QMQTT::SubackType << 4, 2, 42, 0x02);

    EXPECT_EQ(QMQTT::SubackSuccessMaximumQoS2, _packet.returnCode());
}

TEST_F(SubackPacketTestWithStream, typeSubackAndFixedHeaderFlagsZeroIsValid_Test)
{
    streamIntoPacket(QMQTT::SubackType << 4, 2, 42, 0x00);

    EXPECT_TRUE(_packet.isValid());
}

TEST_F(SubackPacketTestWithStream, typeNotSubackAndFixedHeaderFlagsZeroIsInvalid_Test)
{
    streamIntoPacket(QMQTT::ConnectType << 4, 2, 42, 0x00);

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(SubackPacketTestWithStream, typeSubackAndFixedHeaderFlagsNotZeroIsInvalid_Test)
{
    streamIntoPacket((QMQTT::SubackType << 4) | 0x01, 2, 42, 0x00);

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(SubackPacketTestWithStream, returnCodeZeroIsValid_Test)
{
    streamIntoPacket(QMQTT::SubackType << 4, 2, 42, 0x00);

    EXPECT_TRUE(_packet.isValid());
}

TEST_F(SubackPacketTestWithStream, returnCodeOneIsValid_Test)
{
    streamIntoPacket(QMQTT::SubackType << 4, 2, 42, 0x01);

    EXPECT_TRUE(_packet.isValid());
}

TEST_F(SubackPacketTestWithStream, returnCodeTwoIsValid_Test)
{
    streamIntoPacket(QMQTT::SubackType << 4, 2, 42, 0x02);

    EXPECT_TRUE(_packet.isValid());
}

TEST_F(SubackPacketTestWithStream, returnCodeThreeIsInvalid_Test)
{
    streamIntoPacket(QMQTT::SubackType << 4, 2, 42, 0x03);

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(SubackPacketTestWithStream, returnCodeHexEightyIsValid_Test)
{
    streamIntoPacket(QMQTT::SubackType << 4, 2, 42, 0x80);

    EXPECT_TRUE(_packet.isValid());
}
