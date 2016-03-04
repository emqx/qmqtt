#include <qmqtt_pingreqpacket.h>
#include <gtest/gtest.h>

using namespace testing;

class PingreqPacketTest : public Test
{
public:
    PingreqPacketTest() {}
    virtual ~PingreqPacketTest() {}

    QMQTT::PingreqPacket _packet;
};

TEST_F(PingreqPacketTest, defaultConstructorValues_Test)
{
    EXPECT_EQ(QMQTT::PingreqType, _packet.type());
}

TEST_F(PingreqPacketTest, toFrame_Test)
{
    QMQTT::Frame frame = _packet.toFrame();

    EXPECT_EQ(QMQTT::PingreqType, static_cast<QMQTT::PacketType>(frame._header >> 4));

    ASSERT_EQ(0, frame._data.size());
}

TEST_F(PingreqPacketTest, fromFrame_Test)
{
    QMQTT::Frame frame;

    frame._header = QMQTT::PingreqType << 4;

    QMQTT::PingreqPacket packet = QMQTT::PingreqPacket::fromFrame(frame);

    EXPECT_EQ(QMQTT::PingreqType, packet.type());
}

TEST_F(PingreqPacketTest, defaultIsValid_Test)
{
    EXPECT_TRUE(_packet.isValid());
}

TEST_F(PingreqPacketTest, headerReservedBitsNotZeroIsInvalid_Test)
{
    ASSERT_TRUE(_packet.isValid());

    QMQTT::Frame frame = _packet.toFrame();
    frame._header |= 0x01;
    _packet = QMQTT::PingreqPacket::fromFrame(frame);

    EXPECT_FALSE(_packet.isValid());
}
