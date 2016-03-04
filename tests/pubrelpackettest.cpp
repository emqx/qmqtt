#include <qmqtt_pubrelpacket.h>
#include <qmqtt_frame.h>
#include <QBuffer>
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

TEST_F(PubrelPacketTest, toFrame_Test)
{
    _packet.setPacketIdentifier(42);

    QMQTT::Frame frame = _packet.toFrame();

    EXPECT_EQ(QMQTT::PubrelType, static_cast<QMQTT::PacketType>(frame._header >> 4));

    ASSERT_EQ(2, frame._data.size());
    EXPECT_EQ(0, frame._data.at(0));
    EXPECT_EQ(42, frame._data.at(1));
}

TEST_F(PubrelPacketTest, fromFrame_Test)
{
    QMQTT::Frame frame;

    frame._header = QMQTT::PubrelType << 4;

    QBuffer buffer(&frame._data);
    buffer.open(QIODevice::WriteOnly);
    QDataStream stream(&buffer);
    stream << static_cast<quint16>(42);
    buffer.close();

    QMQTT::PubrelPacket packet = QMQTT::PubrelPacket::fromFrame(frame);

    EXPECT_EQ(QMQTT::PubrelType, packet.type());
    EXPECT_EQ(42, packet.packetIdentifier());
}

TEST_F(PubrelPacketTest, defaultIsValid_Test)
{
    EXPECT_TRUE(_packet.isValid());
}

TEST_F(PubrelPacketTest, headerReservedBitsNotTwoIsInvalid_Test)
{
    QMQTT::Frame frame = _packet.toFrame();
    frame._header &= 0xf0;
    _packet = QMQTT::PubrelPacket::fromFrame(frame);

    EXPECT_FALSE(_packet.isValid());
}
