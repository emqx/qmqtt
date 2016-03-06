#include <qmqtt_pubcomppacket.h>
#include <qmqtt_frame.h>
#include <QBuffer>
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

TEST_F(PubcompPacketTest, toFrame_Test)
{
    _packet.setPacketIdentifier(42);

    QMQTT::Frame frame = _packet.toFrame();

    EXPECT_EQ(QMQTT::PubcompType, static_cast<QMQTT::PacketType>(frame._header >> 4));

    ASSERT_EQ(2, frame._data.size());
    EXPECT_EQ(0, frame._data.at(0));
    EXPECT_EQ(42, frame._data.at(1));
}

TEST_F(PubcompPacketTest, fromFrame_Test)
{
    QMQTT::Frame frame;

    frame._header = QMQTT::PubcompType << 4;

    QBuffer buffer(&frame._data);
    buffer.open(QIODevice::WriteOnly);
    QDataStream stream(&buffer);
    stream << static_cast<quint16>(42);
    buffer.close();

    QMQTT::PubcompPacket packet = QMQTT::PubcompPacket::fromFrame(frame);

    EXPECT_EQ(QMQTT::PubcompType, packet.type());
    EXPECT_EQ(42, packet.packetIdentifier());
}

TEST_F(PubcompPacketTest, defaultIsValid_Test)
{
    EXPECT_TRUE(_packet.isValid());
}

TEST_F(PubcompPacketTest, headerReservedBitsNotZeroIsInvalid_Test)
{
    QMQTT::Frame frame = _packet.toFrame();
    frame._header |= 0x01;
    _packet = QMQTT::PubcompPacket::fromFrame(frame);

    EXPECT_FALSE(_packet.isValid());
}
