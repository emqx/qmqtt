#include <qmqtt_subackpacket.h>
#include <qmqtt_frame.h>
#include <QBuffer>
#include <gtest/gtest.h>

using namespace testing;

class SubackPacketTest : public Test
{
public:
    SubackPacketTest() {}
    virtual ~SubackPacketTest() {}

    QMQTT::SubackPacket _packet;
};

TEST_F(SubackPacketTest, defaultConstructorValues_Test)
{
    EXPECT_EQ(QMQTT::SubackType, _packet.type());
    EXPECT_EQ(0, _packet.packetIdentifier());
    EXPECT_TRUE(_packet.returnCodeList().isEmpty());
}

TEST_F(SubackPacketTest, setPacketIdentifier_Test)
{
    _packet.setPacketIdentifier(42);

    EXPECT_EQ(42, _packet.packetIdentifier());
}

TEST_F(SubackPacketTest, setReturnCodeList_Test)
{
    _packet.returnCodeList() << QMQTT::SubackSuccessMaximumQoS1;
    _packet.returnCodeList() << QMQTT::SubackFailure;

    EXPECT_EQ(QMQTT::SubackSuccessMaximumQoS1, _packet.returnCodeList().at(0));
    EXPECT_EQ(QMQTT::SubackFailure, _packet.returnCodeList().at(1));
}

TEST_F(SubackPacketTest, toFrame_Test)
{
    _packet.setPacketIdentifier(42);
    _packet.returnCodeList() << QMQTT::SubackSuccessMaximumQoS2;
    _packet.returnCodeList() << QMQTT::SubackFailure;

    QMQTT::Frame frame = _packet.toFrame();

    EXPECT_EQ(QMQTT::SubackType, static_cast<QMQTT::PacketType>(frame._header >> 4));

    ASSERT_EQ(4, frame._data.size());

    EXPECT_EQ(0, frame._data.at(0));
    EXPECT_EQ(42, frame._data.at(1));
    EXPECT_EQ(QMQTT::SubackSuccessMaximumQoS2,
              static_cast<QMQTT::SubackReturnCodeType>(
              static_cast<quint8>(frame._data.at(2))));
    EXPECT_EQ(QMQTT::SubackFailure,
              static_cast<QMQTT::SubackReturnCodeType>(
              static_cast<quint8>(frame._data.at(3))));
}

TEST_F(SubackPacketTest, fromFrame_Test)
{
    QMQTT::Frame frame;

    frame._header = QMQTT::SubackType << 4;

    QBuffer buffer(&frame._data);
    buffer.open(QIODevice::WriteOnly);
    QDataStream stream(&buffer);
    stream << static_cast<quint16>(42);
    stream << static_cast<quint8>(QMQTT::SubackSuccessMaximumQoS2);
    stream << static_cast<quint8>(QMQTT::SubackFailure);
    buffer.close();

    QMQTT::SubackPacket packet = QMQTT::SubackPacket::fromFrame(frame);

    EXPECT_EQ(QMQTT::SubackType, packet.type());
    EXPECT_EQ(42, packet.packetIdentifier());
    ASSERT_EQ(2, packet.returnCodeList().size());
    EXPECT_EQ(QMQTT::SubackSuccessMaximumQoS2, packet.returnCodeList().at(0));
    EXPECT_EQ(QMQTT::SubackFailure, packet.returnCodeList().at(1));
}

TEST_F(SubackPacketTest, defaultIsInvalid_Test)
{
    ASSERT_TRUE(_packet.returnCodeList().isEmpty());

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(SubackPacketTest, withAtLeastOneReturnCodeIsValid_Test)
{
    _packet.returnCodeList() << QMQTT::SubackFailure;

    EXPECT_TRUE(_packet.isValid());
}

TEST_F(SubackPacketTest, headerReservedBitsNotZeroIsInvalid_Test)
{
    QMQTT::Frame frame = _packet.toFrame();
    frame._header |= 0x01;
    _packet = QMQTT::SubackPacket::fromFrame(frame);

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(SubackPacketTest, invalidReturnCodeTypeIsInvalid_Test)
{
    _packet.returnCodeList() << static_cast<QMQTT::SubackReturnCodeType>(0x03);

    EXPECT_FALSE(_packet.isValid());
}
