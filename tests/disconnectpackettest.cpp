#include "basepackettest.h"
#include <qmqtt_disconnectpacket.h>
#include <gtest/gtest.h>

using namespace testing;

class DisconnectPacketTest : public Test
{
public:
    DisconnectPacketTest() {}
    virtual ~DisconnectPacketTest() {}

    QMQTT::DisconnectPacket _packet;
};

TEST_F(DisconnectPacketTest, defaultConstructorValues_Test)
{
    EXPECT_EQ(QMQTT::DisconnectType, _packet.type());
}

class DisconnectPacketTestWithStream : public BasePacketTest
{
public:
    DisconnectPacketTestWithStream() {}
    virtual ~DisconnectPacketTestWithStream() {}

    QMQTT::DisconnectPacket _packet;

    void streamIntoPacket(
        const quint8 fixedHeader,
        const qint64 remainingLength)
    {
        _stream << fixedHeader;
        writeRemainingLength(remainingLength);
        _buffer.seek(0);
        _stream >> _packet;
    }
};

TEST_F(DisconnectPacketTestWithStream, fixedHeaderTypeWritesDisconnectTypeToStream_Test)
{
    _stream << _packet;

    EXPECT_EQ(QMQTT::DisconnectType,
              static_cast<QMQTT::PacketType>((readUInt8(0) & 0xf0) >> 4));
}

TEST_F(DisconnectPacketTestWithStream, typeDisconnectAndFixedHeaderFlagsZeroIsValid_Test)
{
    streamIntoPacket(QMQTT::DisconnectType << 4, 0);

    EXPECT_TRUE(_packet.isValid());
}

TEST_F(DisconnectPacketTestWithStream, typeNotDisconnectAndFixedHeaderFlagsZeroIsInvalid_Test)
{
    streamIntoPacket(QMQTT::ConnectType << 4, 0);

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(DisconnectPacketTestWithStream, typeDisconnectAndFixedHeaderFlagsNotZeroIsInvalid_Test)
{
    streamIntoPacket((QMQTT::DisconnectType << 4) | 0x01, 0);

    EXPECT_FALSE(_packet.isValid());
}
