#include "basepackettest.h"
#include <qmqtt_pubrecpacket.h>
#include <gtest/gtest.h>

using namespace testing;

class PubrecPacketTest : public Test
{
public:
    PubrecPacketTest() {}
    virtual ~PubrecPacketTest() {}

    QMQTT::PubrecPacket _packet;
};

TEST_F(PubrecPacketTest, defaultConstructorValues_Test)
{
    EXPECT_EQ(QMQTT::PubrecType, _packet.type());
    EXPECT_EQ(0, _packet.packetIdentifier());
}

TEST_F(PubrecPacketTest, setPacketIdentifier_Test)
{
    _packet.setPacketIdentifier(42);
    EXPECT_EQ(42, _packet.packetIdentifier());
}

class PubrecPacketTestWithStream : public BasePacketTest
{
public:
    PubrecPacketTestWithStream() {}
    virtual ~PubrecPacketTestWithStream() {}

    QMQTT::PubrecPacket _packet;

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

TEST_F(PubrecPacketTestWithStream, fixedHeaderTypeWritesPubrecTypeToStream_Test)
{
    _stream << _packet;

    EXPECT_EQ(QMQTT::PubrecType,
              static_cast<QMQTT::PacketType>((readUInt8(0) & 0xf0) >> 4));
}

TEST_F(PubrecPacketTestWithStream, packetIdentifierWritesToStream_Test)
{
    _packet.setPacketIdentifier(42);
    _stream << _packet;

    EXPECT_EQ(42, readUInt16(variableHeaderOffset()));
}

TEST_F(PubrecPacketTestWithStream, packetIdentifierReadsFromStream_Test)
{
    streamIntoPacket(QMQTT::PubrecType << 4, 2, 42);

    EXPECT_EQ(42, _packet.packetIdentifier());
}

TEST_F(PubrecPacketTestWithStream, typePubrecAndFixedHeaderFlagsZeroIsValid_Test)
{
    streamIntoPacket(QMQTT::PubrecType << 4, 2, 42);

    EXPECT_TRUE(_packet.isValid());
}

TEST_F(PubrecPacketTestWithStream, typeNotPubrecAndFixedHeaderFlagsZeroIsInvalid_Test)
{
    streamIntoPacket(QMQTT::ConnectType << 4, 2, 42);

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(PubrecPacketTestWithStream, typePubrecAndFixedHeaderFlagsNotZeroIsInvalid_Test)
{
    streamIntoPacket((QMQTT::PubrecType << 4) | 0x01, 2, 42);

    EXPECT_FALSE(_packet.isValid());
}
