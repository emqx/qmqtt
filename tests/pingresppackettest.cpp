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

TEST_F(PingrespPacketTest, toFrame_Test)
{
    QMQTT::Frame frame = _packet.toFrame();

    EXPECT_EQ(QMQTT::PingrespType, static_cast<QMQTT::PacketType>(frame._header >> 4));

    ASSERT_EQ(0, frame._data.size());
}

TEST_F(PingrespPacketTest, fromFrame_Test)
{
    QMQTT::Frame frame;

    frame._header = QMQTT::PingrespType << 4;

    QMQTT::PingrespPacket packet = QMQTT::PingrespPacket::fromFrame(frame);

    EXPECT_EQ(QMQTT::PingrespType, packet.type());
}

TEST_F(PingrespPacketTest, defaultIsValid_Test)
{
    EXPECT_TRUE(_packet.isValid());
}

TEST_F(PingrespPacketTest, headerReservedBitsNotZeroIsInvalid_Test)
{
    ASSERT_TRUE(_packet.isValid());

    QMQTT::Frame frame = _packet.toFrame();
    frame._header |= 0x01;
    _packet = QMQTT::PingrespPacket::fromFrame(frame);

    EXPECT_FALSE(_packet.isValid());
}
