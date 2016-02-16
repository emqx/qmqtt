#include "basepackettest.h"
#include <qmqtt_pubrelpacket.h>
#include <gtest/gtest.h>

using namespace testing;

class PubrelPacketTest : public Test
{
public:
    PubrelPacketTest() {}
    virtual ~PubrelPacketTest() {}

    QMQTT::PubrelPacket _packet;
};

TEST_F(PubrelPacketTest, defaultConstructorValues_Test)
{
    EXPECT_EQ(QMQTT::PubrelType, _packet.type());
    EXPECT_EQ(0, _packet.packetIdentifier());
}

TEST_F(PubrelPacketTest, setPacketIdentifier_Test)
{
    _packet.setPacketIdentifier(42);
    EXPECT_EQ(42, _packet.packetIdentifier());
}

class PubrelPacketTestWithStream : public BasePacketTest
{
public:
    PubrelPacketTestWithStream() {}
    virtual ~PubrelPacketTestWithStream() {}

    QMQTT::PubrelPacket _packet;

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

TEST_F(PubrelPacketTestWithStream, fixedHeaderTypeWritesPubrelTypeToStream_Test)
{
    _stream << _packet;

    EXPECT_EQ(QMQTT::PubrelType,
              static_cast<QMQTT::PacketType>((readUInt8(0) & 0xf0) >> 4));
}

TEST_F(PubrelPacketTestWithStream, packetIdentifierWritesToStream_Test)
{
    _packet.setPacketIdentifier(42);
    _stream << _packet;

    EXPECT_EQ(42, readUInt16(variableHeaderOffset()));
}

TEST_F(PubrelPacketTestWithStream, packetIdentifierReadsFromStream_Test)
{
    streamIntoPacket(QMQTT::PubrelType << 4, 18, 42);

    EXPECT_EQ(42, _packet.packetIdentifier());
}

TEST_F(PubrelPacketTestWithStream, typePubrelAndFixedHeaderFlagsTwoIsValid_Test)
{
    streamIntoPacket((QMQTT::PubrelType << 4) | 0x02, 2, 42);

    EXPECT_TRUE(_packet.isValid());
}

TEST_F(PubrelPacketTestWithStream, typeNotPubrelAndFixedHeaderFlagsTwoIsInvalid_Test)
{
    streamIntoPacket((QMQTT::ConnectType << 4) | 0x02, 2, 42);

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(PubrelPacketTestWithStream, typePubrelAndFixedHeaderFlagsNotTwoIsInvalid_Test)
{
    streamIntoPacket((QMQTT::PubrelType << 4) | 0x01, 2, 42);

    EXPECT_FALSE(_packet.isValid());
}
