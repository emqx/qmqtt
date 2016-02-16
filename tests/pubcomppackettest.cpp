#include "basepackettest.h"
#include <qmqtt_pubcomppacket.h>
#include <gtest/gtest.h>

using namespace testing;

class PubcompPacketTest : public Test
{
public:
    PubcompPacketTest() {}
    virtual ~PubcompPacketTest() {}

    QMQTT::PubcompPacket _packet;
};

TEST_F(PubcompPacketTest, defaultConstructorValues_Test)
{
    EXPECT_EQ(QMQTT::PubcompType, _packet.type());
    EXPECT_EQ(0, _packet.packetIdentifier());
}

TEST_F(PubcompPacketTest, setPacketIdentifier_Test)
{
    _packet.setPacketIdentifier(42);
    EXPECT_EQ(42, _packet.packetIdentifier());
}

class PubcompPacketTestWithStream : public BasePacketTest
{
public:
    PubcompPacketTestWithStream() {}
    virtual ~PubcompPacketTestWithStream() {}

    QMQTT::PubcompPacket _packet;

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

TEST_F(PubcompPacketTestWithStream, fixedHeaderTypeWritesPubcompTypeToStream_Test)
{
    _stream << _packet;

    EXPECT_EQ(QMQTT::PubcompType,
              static_cast<QMQTT::PacketType>((readUInt8(0) & 0xf0) >> 4));
}

TEST_F(PubcompPacketTestWithStream, packetIdentifierWritesToStream_Test)
{
    _packet.setPacketIdentifier(42);
    _stream << _packet;

    EXPECT_EQ(42, readUInt16(variableHeaderOffset()));
}

TEST_F(PubcompPacketTestWithStream, packetIdentifierReadsFromStream_Test)
{
    streamIntoPacket(QMQTT::PubcompType << 4, 2, 42);

    EXPECT_EQ(42, _packet.packetIdentifier());
}

TEST_F(PubcompPacketTestWithStream, typePubcompAndFixedHeaderFlagsZeroIsValid_Test)
{
    streamIntoPacket(QMQTT::PubcompType << 4, 2, 42);

    EXPECT_TRUE(_packet.isValid());
}

TEST_F(PubcompPacketTestWithStream, typeNotPubcompAndFixedHeaderFlagsZeroIsInvalid_Test)
{
    streamIntoPacket(QMQTT::ConnectType << 4, 2, 42);

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(PubcompPacketTestWithStream, typePubcompAndFixedHeaderFlagsNotZeroIsInvalid_Test)
{
    streamIntoPacket((QMQTT::PubcompType << 4) | 0x01, 2, 42);

    EXPECT_FALSE(_packet.isValid());
}
