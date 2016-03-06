#include "basepackettest.h"
#include <qmqtt_connackpacket.h>
#include <gtest/gtest.h>

using namespace testing;

class ConnackPacketTest : public Test
{
public:
    ConnackPacketTest() {}
    virtual ~ConnackPacketTest() {}

    QMQTT::ConnackPacket _packet;
};

TEST_F(ConnackPacketTest, defaultConstructorValues_Test)
{
    EXPECT_EQ(QMQTT::ConnackType, _packet.type());
    EXPECT_FALSE(_packet.sessionPresent());
    EXPECT_EQ(QMQTT::ConnectionAccepted, _packet.connectReturnCode());
}

TEST_F(ConnackPacketTest, setSessionPresent_Test)
{
    _packet.setSessionPresent(true);

    EXPECT_TRUE(_packet.sessionPresent());
}

TEST_F(ConnackPacketTest, setConnectReturnCode_Test)
{
    _packet.setConnectReturnCode(QMQTT::ConnectionRefusedUnacceptableProtocolVersion);

    EXPECT_EQ(QMQTT::ConnectionRefusedUnacceptableProtocolVersion, _packet.connectReturnCode());
}

TEST_F(ConnackPacketTest, toFrame_Test)
{
    _packet.setSessionPresent(true);
    _packet.setConnectReturnCode(QMQTT::ConnectionRefusedUnacceptableProtocolVersion);

    QMQTT::Frame frame = _packet.toFrame();

    EXPECT_EQ(QMQTT::ConnackType << 4, frame._header);
    ASSERT_EQ(2, frame._data.size());
    EXPECT_EQ(1, frame._data.at(0));
    EXPECT_EQ(static_cast<quint8>(QMQTT::ConnectionRefusedUnacceptableProtocolVersion), frame._data.at(1));
}

TEST_F(ConnackPacketTest, fromFrame_Test)
{
    QMQTT::Frame frame;
    frame._header = QMQTT::ConnackType << 4;
    QBuffer buffer(&frame._data);
    buffer.open(QIODevice::WriteOnly);
    QDataStream stream(&buffer);
    stream << static_cast<quint8>(1);
    stream << static_cast<quint8>(QMQTT::ConnectionRefusedUnacceptableProtocolVersion);
    buffer.close();

    QMQTT::ConnackPacket packet = QMQTT::ConnackPacket::fromFrame(frame);

    EXPECT_TRUE(packet.sessionPresent());
    EXPECT_EQ(QMQTT::ConnectionRefusedUnacceptableProtocolVersion, packet.connectReturnCode());
}

TEST_F(ConnackPacketTest, defaultIsValid_Test)
{
    EXPECT_TRUE(_packet.isValid());
}

TEST_F(ConnackPacketTest, headerReservedBitsNotZeroIsInvalid_Test)
{
    ASSERT_TRUE(_packet.isValid());

    QMQTT::Frame frame = _packet.toFrame();
    frame._header |= 0x01;
    _packet = QMQTT::ConnackPacket::fromFrame(frame);

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(ConnackPacketTest, connectAcknowledgeFlagsReservedBitsNotZeroIsInvalid_Test)
{
    ASSERT_TRUE(_packet.isValid());

    QMQTT::Frame frame = _packet.toFrame();
    frame._data[0] = frame._data.at(0) | 0x02;
    _packet = QMQTT::ConnackPacket::fromFrame(frame);

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(ConnackPacketTest, sessionPresentFlagFalseAndConnectReturnCodeNotAcceptedIsInvalid_Test)
{
    ASSERT_TRUE(_packet.isValid());

    QMQTT::Frame frame = _packet.toFrame();
    ASSERT_EQ(0, frame._data.at(0));
    frame._data[1] = QMQTT::ConnectionRefusedUnacceptableProtocolVersion;
    _packet = QMQTT::ConnackPacket::fromFrame(frame);

    EXPECT_FALSE(_packet.isValid());
}
