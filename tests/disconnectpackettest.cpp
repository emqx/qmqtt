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

TEST_F(DisconnectPacketTest, toFrame_Test)
{
    QMQTT::Frame frame = _packet.toFrame();

    EXPECT_EQ(QMQTT::DisconnectType, static_cast<QMQTT::PacketType>(frame._header >> 4));

    ASSERT_EQ(0, frame._data.size());
}

TEST_F(DisconnectPacketTest, fromFrame_Test)
{
    QMQTT::Frame frame;

    frame._header = QMQTT::DisconnectType << 4;

    QMQTT::DisconnectPacket packet = QMQTT::DisconnectPacket::fromFrame(frame);

    EXPECT_EQ(QMQTT::DisconnectType, packet.type());
}

TEST_F(DisconnectPacketTest, defaultIsValid_Test)
{
    EXPECT_TRUE(_packet.isValid());
}

TEST_F(DisconnectPacketTest, headerReservedBitsNotZeroIsInvalid_Test)
{
    ASSERT_TRUE(_packet.isValid());

    QMQTT::Frame frame = _packet.toFrame();
    frame._header |= 0x01;
    _packet = QMQTT::DisconnectPacket::fromFrame(frame);

    EXPECT_FALSE(_packet.isValid());
}
