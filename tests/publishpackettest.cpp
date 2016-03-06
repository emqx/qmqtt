#include "testutils.h"
#include <qmqtt_publishpacket.h>
#include <QBuffer>
#include <gtest/gtest.h>

using namespace testing;

class PublishPacketTest : public Test
{
public:
    PublishPacketTest() {}
    virtual ~PublishPacketTest() {}

    QMQTT::PublishPacket _packet;
};

TEST_F(PublishPacketTest, defaultConstructorValues_Test)
{
    EXPECT_EQ(QMQTT::PublishType, _packet.type());
    EXPECT_FALSE(_packet.dupFlag());
    EXPECT_EQ(0, _packet.qos());
    EXPECT_FALSE(_packet.retainFlag());
    EXPECT_TRUE(_packet.topicName().isEmpty());
    EXPECT_EQ(0, _packet.packetIdentifier());
    EXPECT_TRUE(_packet.payload().isEmpty());
    EXPECT_TRUE(_packet.isValid());
}

TEST_F(PublishPacketTest, setDupFlag_Test)
{
    _packet.setDupFlag(true);

    EXPECT_TRUE(_packet.dupFlag());
}

TEST_F(PublishPacketTest, setQos_Test)
{
    _packet.setQos(QMQTT::Qos2);

    EXPECT_EQ(QMQTT::Qos2, _packet.qos());
}

TEST_F(PublishPacketTest, setRetainFlag_Test)
{
    _packet.setRetainFlag(true);

    EXPECT_TRUE(_packet.retainFlag());
}

TEST_F(PublishPacketTest, setTopicName_Test)
{
    _packet.setTopicName("topic");

    EXPECT_EQ("topic", _packet.topicName());
}

TEST_F(PublishPacketTest, setPacketIdentifier_Test)
{
    _packet.setPacketIdentifier(42);

    EXPECT_EQ(42, _packet.packetIdentifier());
}

TEST_F(PublishPacketTest, setPayload_Test)
{
    _packet.setPayload(QString("payload").toUtf8());

    EXPECT_EQ("payload", QString::fromUtf8(_packet.payload()));
}

TEST_F(PublishPacketTest, toFrame_Test)
{
    _packet.setDupFlag(true);
    _packet.setQos(QMQTT::Qos2);
    _packet.setRetainFlag(true);
    _packet.setTopicName("topic");
    _packet.setPacketIdentifier(42);
    _packet.setPayload(QString("payload").toUtf8());

    QMQTT::Frame frame = _packet.toFrame();

    EXPECT_EQ(QMQTT::PublishType, static_cast<QMQTT::PacketType>(frame._header >> 4));
    EXPECT_NE(0, frame._header & 0x08);
    EXPECT_EQ(QMQTT::Qos2, static_cast<QMQTT::QosType>((frame._header & 0x06) >> 1));
    EXPECT_NE(0, frame._header & 0x01);

    ASSERT_EQ(16, frame._data.size());
    EXPECT_EQ(0, frame._data.at(0));
    EXPECT_EQ(5, frame._data.at(1));
    EXPECT_EQ("topic", QString::fromUtf8(frame._data.mid(2, 5)));
    EXPECT_EQ(0, frame._data.at(7));
    EXPECT_EQ(42, frame._data.at(8));
    EXPECT_EQ("payload", QString::fromUtf8(frame._data.mid(9, 7)));
}

TEST_F(PublishPacketTest, fromFrame_Test)
{
    QMQTT::Frame frame;
    frame._header = (QMQTT::PublishType << 4) | 0x0d;

    QBuffer buffer(&frame._data);
    buffer.open(QIODevice::WriteOnly);
    QDataStream stream(&buffer);

    TestUtils::writeString(stream, "topic");
    stream << static_cast<quint16>(42);
    QByteArray array(QString("payload").toUtf8());
    stream.writeRawData(array.constData(), array.size());

    buffer.close();

    QMQTT::PublishPacket packet = QMQTT::PublishPacket::fromFrame(frame);

    EXPECT_EQ(QMQTT::PublishType, packet.type());
    EXPECT_TRUE(packet.dupFlag());
    EXPECT_EQ(QMQTT::Qos2, packet.qos());
    EXPECT_TRUE(packet.retainFlag());
    EXPECT_EQ("topic",  packet.topicName());
    EXPECT_EQ(42, packet.packetIdentifier());
    EXPECT_EQ("payload", QString::fromUtf8(packet.payload()));
}

TEST_F(PublishPacketTest, defaultIsValid_Test)
{
    EXPECT_TRUE(_packet.isValid());
}

TEST_F(PublishPacketTest, qosZeroAndDupFlagTrueIsInvalid_Test)
{
    _packet.setDupFlag(true);
    EXPECT_FALSE(_packet.isValid());
}

TEST_F(PublishPacketTest, qosThreeIsInvalid_Test)
{
    QMQTT::Frame frame = _packet.toFrame();
    frame._header |= 0x06;
    _packet = QMQTT::PublishPacket::fromFrame(frame);

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(PublishPacketTest, topicNameWithHashMarkIsInvalid_Test)
{
    _packet.setTopicName("#");

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(PublishPacketTest, topicNameWithPlusSignIsInvalid_Test)
{
    _packet.setTopicName("+");

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(PublishPacketTest, qosNotZeroWithPacketIdentifierZeroIsInvalid_Test)
{
    _packet.setQos(QMQTT::Qos2);
    ASSERT_EQ(0, _packet.packetIdentifier());

    EXPECT_FALSE(_packet.isValid());
}
