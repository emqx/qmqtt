#include <qmqtt_unsubscribepacket.h>
#include <qmqtt_frame.h>
#include <QBuffer>
#include <QDataStream>
#include <gtest/gtest.h>

using namespace testing;

class UnsubscribePacketTest : public Test
{
public:
    UnsubscribePacketTest() {}
    virtual ~UnsubscribePacketTest() {}

    QMQTT::UnsubscribePacket _packet;
};

TEST_F(UnsubscribePacketTest, defaultConstructorValues_Test)
{
    EXPECT_EQ(QMQTT::UnsubscribeType, _packet.type());
    EXPECT_EQ(0, _packet.packetIdentifier());
    EXPECT_TRUE(_packet.topicFilterList().isEmpty());
}

TEST_F(UnsubscribePacketTest, setPacketIdentifier_Test)
{
    _packet.setPacketIdentifier(42);

    EXPECT_EQ(42, _packet.packetIdentifier());
}

TEST_F(UnsubscribePacketTest, setTopicFilterList_Test)
{
    _packet.topicFilterList() << "topic1";
    _packet.topicFilterList() << "topic2";

    EXPECT_EQ("topic1", _packet.topicFilterList().at(0));
    EXPECT_EQ("topic2", _packet.topicFilterList().at(1));
}

TEST_F(UnsubscribePacketTest, toFrame_Test)
{
    _packet.setPacketIdentifier(42);
    _packet.topicFilterList() << "topic1";
    _packet.topicFilterList() << "topic2";

    QMQTT::Frame frame = _packet.toFrame();

    EXPECT_EQ(QMQTT::UnsubscribeType, static_cast<QMQTT::PacketType>(frame._header >> 4));

    ASSERT_EQ(18, frame._data.size());
    EXPECT_EQ(0, frame._data.at(0));
    EXPECT_EQ(42, frame._data.at(1));
    EXPECT_EQ(0, frame._data.at(2));
    EXPECT_EQ(6, frame._data.at(3));
    EXPECT_EQ("topic1", QString::fromUtf8(frame._data.mid(4, 6)));
    EXPECT_EQ(0, frame._data.at(10));
    EXPECT_EQ(6, frame._data.at(11));
    EXPECT_EQ("topic2", QString::fromUtf8(frame._data.mid(12, 6)));
}

TEST_F(UnsubscribePacketTest, fromFrame_Test)
{
    QMQTT::Frame frame;

    frame._header = QMQTT::SubackType << 4;

    QBuffer buffer(&frame._data);
    buffer.open(QIODevice::WriteOnly);
    QDataStream stream(&buffer);
    stream << static_cast<quint16>(42);
    QByteArray array(QString("topic1").toUtf8());
    stream << static_cast<quint16>(array.size());
    stream.writeRawData(array.constData(), array.size());
    array = QString("topic2").toUtf8();
    stream << static_cast<quint16>(array.size());
    stream.writeRawData(array.constData(), array.size());
    buffer.close();

    QMQTT::UnsubscribePacket packet = QMQTT::UnsubscribePacket::fromFrame(frame);

    EXPECT_EQ(QMQTT::UnsubscribeType, packet.type());
    EXPECT_EQ(42, packet.packetIdentifier());
    ASSERT_EQ(2, packet.topicFilterList().size());
    EXPECT_EQ("topic1", packet.topicFilterList().at(0));
    EXPECT_EQ("topic2", packet.topicFilterList().at(1));
}

TEST_F(UnsubscribePacketTest, defaultIsInvalid_Test)
{
    ASSERT_TRUE(_packet.topicFilterList().isEmpty());

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(UnsubscribePacketTest, withAtLeastOneTopicfilterIsValid_Test)
{
    _packet.topicFilterList() << "topic1";

    EXPECT_TRUE(_packet.isValid());
}

TEST_F(UnsubscribePacketTest, headerReservedBitsNotZeroIsInvalid_Test)
{
    _packet.topicFilterList() << "topic1";
    ASSERT_TRUE(_packet.isValid());

    QMQTT::Frame frame = _packet.toFrame();
    frame._header |= 0x01;
    _packet = QMQTT::UnsubscribePacket::fromFrame(frame);

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(UnsubscribePacketTest, emptyTopicFilterIsInvalid_Test)
{
    _packet.topicFilterList() << "";

    EXPECT_FALSE(_packet.isValid());
}
