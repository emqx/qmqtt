#include "basepackettest.h"
#include <qmqtt_subscribepacket.h>
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

class SubscribePacketTestWithStream : public BasePacketTest
{
public:
    SubscribePacketTestWithStream() {}
    virtual ~SubscribePacketTestWithStream() {}

    QMQTT::SubscribePacket _packet;

    void streamIntoPacket(
        const quint8 fixedHeader,
        const qint64 remainingLength,
        const quint16 packetIdentifier,
        const QString& firstTopic = QString(),
        const quint8 firstQos = 0,
        const QString& secondTopic = QString(),
        const quint8 secondQos = 0)
    {
        _stream << fixedHeader;
        writeRemainingLength(remainingLength);
        _stream << packetIdentifier;
        if (!firstTopic.isNull())
        {
            writeString(firstTopic);
            _stream << firstQos;
        }
        if (!secondTopic.isNull())
        {
            writeString(secondTopic);
            _stream << secondQos;
        }

        _buffer.seek(0);
        _stream >> _packet;
    }
};

TEST_F(SubscribePacketTestWithStream, fixedHeaderTypeWritesSubscribeTypeToStream_Test)
{
    _stream << _packet;

    EXPECT_EQ(QMQTT::SubscribeType,
              static_cast<QMQTT::PacketType>((readUInt8(0) & 0xf0) >> 4));
}

TEST_F(SubscribePacketTestWithStream, fixedHeaderFlagsWritesHexTwoToStream_Test)
{
    _stream << _packet;

    EXPECT_EQ(0x02, readUInt8(0) & 0x0f);
}

TEST_F(SubscribePacketTestWithStream, packetIdentifierWritesToStream_Test)
{
    _packet.setPacketIdentifier(42);
    _stream << _packet;

    EXPECT_EQ(42, readUInt16(variableHeaderOffset()));
}

TEST_F(SubscribePacketTestWithStream, topicListWritesToStream_Test)
{
    _packet.setPacketIdentifier(42);
    QMQTT::Subscription first;
    first._topicFilter = "first";
    first._requestedQos = 1;
    QMQTT::Subscription second;
    second._topicFilter = "second";
    second._requestedQos = 2;
    _packet.setSubscriptionList(QMQTT::SubscriptionList() << first << second);
    _stream << _packet;

    EXPECT_EQ("first", readString(variableHeaderOffset() + 2));
    EXPECT_EQ(1, readUInt8());
    EXPECT_EQ("second", readString());
    EXPECT_EQ(2, readUInt8());
}

TEST_F(SubscribePacketTestWithStream, packetIdentifierReadsFromStream_Test)
{
    streamIntoPacket(QMQTT::SubscribeType << 4, 10, 42, "first", 1);

    EXPECT_EQ(42, _packet.packetIdentifier());
}

TEST_F(SubscribePacketTestWithStream, topicListReadsFromStream_Test)
{
    streamIntoPacket(QMQTT::SubscribeType << 4, 17, 42, "first", 1, "second", 2);

    EXPECT_EQ(21, _byteArray.size());
    EXPECT_EQ(2, _packet.subscriptionList().size());
    EXPECT_EQ("first", _packet.subscriptionList().at(0)._topicFilter);
    EXPECT_EQ(1, _packet.subscriptionList().at(0)._requestedQos);
    EXPECT_EQ("second", _packet.subscriptionList().at(1)._topicFilter);
    EXPECT_EQ(2, _packet.subscriptionList().at(1)._requestedQos);
}

TEST_F(SubscribePacketTestWithStream, typeSubscribeAndFixedHeaderFlagsTwoIsValid_Test)
{
    streamIntoPacket((QMQTT::SubscribeType << 4) | 0x02, 17, 42, "first", 1, "second", 2);

    EXPECT_TRUE(_packet.isValid());
}

TEST_F(SubscribePacketTestWithStream, typeNotSubscribeAndFixedHeaderFlagsTwoIsInvalid_Test)
{
    streamIntoPacket((QMQTT::ConnectType << 4) | 0x02, 17, 42, "first", 1, "second", 2);

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(SubscribePacketTestWithStream, typeSubscribeAndFixedHeaderFlagsNotTwoIsInvalid_Test)
{
    streamIntoPacket((QMQTT::SubscribeType << 4) | 0x01, 17, 42, "first", 1, "second", 2);

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(SubscribePacketTestWithStream, emptyTopicListIsInvalid_Test)
{
    streamIntoPacket((QMQTT::SubscribeType << 4) | 0x02, 2, 42);

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(SubscribePacketTestWithStream, requestQosThreeIsInvalid_Test)
{
    streamIntoPacket((QMQTT::SubscribeType << 4) | 0x02, 10, 42, "first", 3);

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(SubscribePacketTestWithStream, emptyTopicNameIsInvalid_Test)
{
    streamIntoPacket((QMQTT::SubscribeType << 4) | 0x02, 10, 42, "", 0);

    EXPECT_FALSE(_packet.isValid());
}
