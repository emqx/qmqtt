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

TEST_F(PubrecPacketTest, toFrame_Test)
{
    _packet.setPacketIdentifier(42);

    QMQTT::Frame frame = _packet.toFrame();

    EXPECT_EQ(QMQTT::PubrecType, static_cast<QMQTT::PacketType>(frame._header >> 4));

    ASSERT_EQ(2, frame._data.size());
    EXPECT_EQ(0, frame._data.at(0));
    EXPECT_EQ(42, frame._data.at(1));
}

TEST_F(PubrecPacketTest, fromFrame_Test)
{
    QMQTT::Frame frame;

    frame._header = QMQTT::PubrecType << 4;

    QBuffer buffer(&frame._data);
    buffer.open(QIODevice::WriteOnly);
    QDataStream stream(&buffer);
    stream << static_cast<quint16>(42);
    buffer.close();

    QMQTT::PubrecPacket packet = QMQTT::PubrecPacket::fromFrame(frame);

    EXPECT_EQ(QMQTT::PubrecType, packet.type());
    EXPECT_EQ(42, packet.packetIdentifier());
}

TEST_F(PubrecPacketTest, defaultIsValid_Test)
{
    EXPECT_TRUE(_packet.isValid());
}

TEST_F(PubrecPacketTest, headerReservedBitsNotZeroIsInvalid_Test)
{
    QMQTT::Frame frame = _packet.toFrame();
    frame._header &= 0xf0;
    frame._header |= 0x01;
    _packet = QMQTT::PubrecPacket::fromFrame(frame);

    EXPECT_FALSE(_packet.isValid());
}
