#include <qmqtt_unsubackpacket.h>
#include <QBuffer>
#include <gtest/gtest.h>

using namespace testing;

class UnsubackPacketTest : public Test
{
public:
    UnsubackPacketTest() {}
    virtual ~UnsubackPacketTest() {}

    QMQTT::UnsubackPacket _packet;
};

TEST_F(UnsubackPacketTest, defaultConstructorValues_Test)
{
    EXPECT_EQ(QMQTT::UnsubackType, _packet.type());
    EXPECT_EQ(0, _packet.packetIdentifier());
}

TEST_F(UnsubackPacketTest, setPacketIdentifier_Test)
{
    _packet.setPacketIdentifier(42);

    EXPECT_EQ(42, _packet.packetIdentifier());
}

TEST_F(UnsubackPacketTest, toFrame_Test)
{
    _packet.setPacketIdentifier(42);

    QMQTT::Frame frame = _packet.toFrame();

    EXPECT_EQ(QMQTT::UnsubackType, static_cast<QMQTT::PacketType>(frame._header >> 4));

    ASSERT_EQ(2, frame._data.size());
    EXPECT_EQ(0, frame._data.at(0));
    EXPECT_EQ(42, frame._data.at(1));
}

TEST_F(UnsubackPacketTest, fromFrame_Test)
{
    QMQTT::Frame frame;

    frame._header = QMQTT::UnsubackType << 4;

    QBuffer buffer(&frame._data);
    buffer.open(QIODevice::WriteOnly);
    QDataStream stream(&buffer);
    stream << static_cast<quint16>(42);
    buffer.close();

    QMQTT::UnsubackPacket packet = QMQTT::UnsubackPacket::fromFrame(frame);

    EXPECT_EQ(QMQTT::UnsubackType, packet.type());
    EXPECT_EQ(42, packet.packetIdentifier());
}

TEST_F(UnsubackPacketTest, defaultIsValid_Test)
{
    EXPECT_TRUE(_packet.isValid());
}

TEST_F(UnsubackPacketTest, headerReservedBitsNotZeroIsInvalid_Test)
{
    ASSERT_TRUE(_packet.isValid());

    QMQTT::Frame frame = _packet.toFrame();
    frame._header |= 0x01;
    _packet = QMQTT::UnsubackPacket::fromFrame(frame);

    EXPECT_FALSE(_packet.isValid());
}
