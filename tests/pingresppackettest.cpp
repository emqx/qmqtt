#include "basepackettest.h"
#include <qmqtt_pingresppacket.h>
#include <gtest/gtest.h>

using namespace testing;

class PingrespPacketTest : public Test
{
public:
    PingrespPacketTest() {}
    virtual ~PingrespPacketTest() {}

    QMQTT::PingrespPacket _packet;
};

TEST_F(PingrespPacketTest, defaultConstructorValues_Test)
{
    EXPECT_EQ(QMQTT::PingrespType, _packet.type());
}

class PingrespPacketTestWithStream : public BasePacketTest
{
public:
    PingrespPacketTestWithStream() {}
    virtual ~PingrespPacketTestWithStream() {}

    QMQTT::PingrespPacket _packet;

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

TEST_F(PingrespPacketTestWithStream, fixedHeaderTypeWritesPingrespTypeToStream_Test)
{
    _stream << _packet;

    EXPECT_EQ(QMQTT::PingrespType,
              static_cast<QMQTT::PacketType>((readUInt8(0) & 0xf0) >> 4));
}

TEST_F(PingrespPacketTestWithStream, typePingrespAndFixedHeaderFlagsZeroIsValid_Test)
{
    streamIntoPacket(QMQTT::PingrespType << 4, 0);

    EXPECT_TRUE(_packet.isValid());
}

TEST_F(PingrespPacketTestWithStream, typeNotPingrespAndFixedHeaderFlagsZeroIsInvalid_Test)
{
    streamIntoPacket(QMQTT::ConnectType << 4, 0);

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(PingrespPacketTestWithStream, typePingrespAndFixedHeaderFlagsNotZeroIsInvalid_Test)
{
    streamIntoPacket((QMQTT::PingrespType << 4) | 0x01, 0);

    EXPECT_FALSE(_packet.isValid());
}
