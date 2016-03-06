#include <qmqtt_subscribepacket.h>
#include <qmqtt_frame.h>
#include <QBuffer>
#include <gtest/gtest.h>

using namespace testing;

class SubscribePacketTest : public Test
{
public:
    SubscribePacketTest() {}
    virtual ~SubscribePacketTest() {}

    QMQTT::SubscribePacket _packet;
};

TEST_F(SubscribePacketTest, defaultConstructorValues_Test)
{
    EXPECT_EQ(QMQTT::SubscribeType, _packet.type());
    EXPECT_EQ(0, _packet.packetIdentifier());
}

TEST_F(SubscribePacketTest, setPacketIdentifier_Test)
{
    _packet.setPacketIdentifier(42);

    EXPECT_EQ(42, _packet.packetIdentifier());
}

TEST_F(SubscribePacketTest, toFrame_Test)
{
    _packet.setPacketIdentifier(42);
    QMQTT::Subscription sub;
    sub._topicFilter = "topic1";
    sub._requestedQos = QMQTT::Qos1;
    _packet.subscriptionList() << sub;
    sub._topicFilter = "topic2";
    sub._requestedQos = QMQTT::Qos2;
    _packet.subscriptionList() << sub;

    QMQTT::Frame frame = _packet.toFrame();

    EXPECT_EQ(QMQTT::SubscribeType, static_cast<QMQTT::PacketType>(frame._header >> 4));

    ASSERT_EQ(20, frame._data.size());

    EXPECT_EQ(0, frame._data.at(0));
    EXPECT_EQ(42, frame._data.at(1));

    EXPECT_EQ(0, frame._data.at(2));
    EXPECT_EQ(6, frame._data.at(3));
    EXPECT_EQ("topic1", QString::fromUtf8(frame._data.mid(4, 6)));
    EXPECT_EQ(QMQTT::Qos1,
              static_cast<QMQTT::QosType>(
              static_cast<quint8>(frame._data.at(10))));
    EXPECT_EQ(0, frame._data.at(11));
    EXPECT_EQ(6, frame._data.at(12));
    EXPECT_EQ("topic2", QString::fromUtf8(frame._data.mid(13, 6)));
    EXPECT_EQ(QMQTT::Qos2,
              static_cast<QMQTT::QosType>(
              static_cast<quint8>(frame._data.at(19))));
}

TEST_F(SubscribePacketTest, fromFrame_Test)
{
    QMQTT::Frame frame;

    frame._header = QMQTT::SubscribeType << 4;

    QBuffer buffer(&frame._data);
    buffer.open(QIODevice::WriteOnly);
    QDataStream stream(&buffer);
    stream << static_cast<quint16>(42);
    stream << static_cast<quint16>(6);
    QByteArray array(QString("topic1").toUtf8());
    stream.writeRawData(array.constData(), array.size());
    stream << static_cast<quint8>(QMQTT::Qos1);
    stream << static_cast<quint16>(6);
    array = QString("topic2").toUtf8();
    stream.writeRawData(array.constData(), array.size());
    stream << static_cast<quint8>(QMQTT::Qos2);
    buffer.close();

    QMQTT::SubscribePacket packet = QMQTT::SubscribePacket::fromFrame(frame);

    EXPECT_EQ(QMQTT::SubscribeType, packet.type());
    EXPECT_EQ(42, packet.packetIdentifier());
    EXPECT_EQ(2, packet.subscriptionList().size());
    EXPECT_EQ("topic1", packet.subscriptionList().at(0)._topicFilter);
    EXPECT_EQ(QMQTT::Qos1, packet.subscriptionList().at(0)._requestedQos);
    EXPECT_EQ("topic2", packet.subscriptionList().at(1)._topicFilter);
    EXPECT_EQ(QMQTT::Qos2, packet.subscriptionList().at(1)._requestedQos);
}

TEST_F(SubscribePacketTest, defaultIsInvalid_Test)
{
    EXPECT_FALSE(_packet.isValid());
}

TEST_F(SubscribePacketTest, withAtLeastOneSubscriptionIsValid_Test)
{
    QMQTT::Subscription sub;
    sub._topicFilter = "topic1";
    sub._requestedQos = QMQTT::Qos2;
    _packet.subscriptionList() << sub;

    EXPECT_TRUE(_packet.isValid());
}

TEST_F(SubscribePacketTest, headerReservedBitsNotZeroIsInvalid_Test)
{
    QMQTT::Frame frame = _packet.toFrame();
    frame._header |= 0x01;
    _packet = QMQTT::SubscribePacket::fromFrame(frame);

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(SubscribePacketTest, emptySubscriptionFilterIsInvalid_Test)
{
    QMQTT::Subscription sub;
    sub._topicFilter = "";
    sub._requestedQos = QMQTT::Qos2;
    _packet.subscriptionList() << sub;

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(SubscribePacketTest, QosThreeSubscriptionIsInvalid_Test)
{
    QMQTT::Subscription sub;
    sub._topicFilter = "topic1";
    sub._requestedQos = static_cast<QMQTT::QosType>(3);
    _packet.subscriptionList() << sub;

    EXPECT_FALSE(_packet.isValid());
}
