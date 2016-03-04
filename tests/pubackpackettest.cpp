#include <qmqtt_pubackpacket.h>
#include <gtest/gtest.h>
#include <QBuffer>

using namespace testing;

class PubackPacketTest : public Test
{
public:
    PubackPacketTest() {}
    virtual ~PubackPacketTest() {}

    QMQTT::PubackPacket _packet;
};

TEST_F(PubackPacketTest, defaultConstructorValues_Test)
{
    EXPECT_EQ(QMQTT::PubackType, _packet.type());
    EXPECT_EQ(0, _packet.packetIdentifier());
}

TEST_F(PubackPacketTest, setPacketIdentifier_Test)
{
    _packet.setPacketIdentifier(42);

    EXPECT_EQ(42, _packet.packetIdentifier());
}

TEST_F(PubackPacketTest, toFrame_Test)
{
    _packet.setPacketIdentifier(42);

    QMQTT::Frame frame = _packet.toFrame();

    EXPECT_EQ(QMQTT::PubackType, static_cast<QMQTT::PacketType>(frame._header >> 4));

    ASSERT_EQ(2, frame._data.size());
    EXPECT_EQ(0, frame._data.at(0));
    EXPECT_EQ(42, frame._data.at(1));
}

TEST_F(PubackPacketTest, fromFrame_Test)
{
    QMQTT::Frame frame;

    frame._header = QMQTT::PubackType << 4;

    QBuffer buffer(&frame._data);
    buffer.open(QIODevice::WriteOnly);
    QDataStream stream(&buffer);
    stream << static_cast<quint16>(42);
    buffer.close();

    QMQTT::PubackPacket packet = QMQTT::PubackPacket::fromFrame(frame);

    EXPECT_EQ(QMQTT::PubackType, packet.type());
    EXPECT_EQ(42, packet.packetIdentifier());
}

TEST_F(PubackPacketTest, defaultIsValid_Test)
{
    EXPECT_TRUE(_packet.isValid());
}

TEST_F(PubackPacketTest, headerReservedBitsNotZeroIsInvalid_Test)
{
    QMQTT::Frame frame = _packet.toFrame();
    frame._header &= 0xf0;
    frame._header |= 0x01;
    _packet = QMQTT::PubackPacket::fromFrame(frame);

    EXPECT_FALSE(_packet.isValid());
}
