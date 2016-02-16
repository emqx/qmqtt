#include "basepackettest.h"
#include <qmqtt_unsubscribepacket.h>
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
}

TEST_F(UnsubscribePacketTest, setPacketIdentifier_Test)
{
    _packet.setPacketIdentifier(42);
    EXPECT_EQ(42, _packet.packetIdentifier());
}

class UnsubscribePacketTestWithStream : public BasePacketTest
{
public:
    UnsubscribePacketTestWithStream() {}
    virtual ~UnsubscribePacketTestWithStream() {}

    QMQTT::UnsubscribePacket _packet;

    void streamIntoPacket(
        const quint8 fixedHeader,
        const qint64 remainingLength,
        const quint16 packetIdentifier,
        const QString& firstTopic = QString(),
        const QString& secondTopic = QString())
    {
        _stream << fixedHeader;
        writeRemainingLength(remainingLength);
        _stream << packetIdentifier;
        if (!firstTopic.isNull())
        {
            writeString(firstTopic);
        }
        if (!secondTopic.isNull())
        {
            writeString(secondTopic);
        }

        _buffer.seek(0);
        _stream >> _packet;
    }
};

TEST_F(UnsubscribePacketTestWithStream, fixedHeaderTypeWritesSubscribeTypeToStream_Test)
{
    _stream << _packet;

    EXPECT_EQ(QMQTT::UnsubscribeType,
              static_cast<QMQTT::PacketType>((readUInt8(0) & 0xf0) >> 4));
}

TEST_F(UnsubscribePacketTestWithStream, fixedHeaderFlagsWritesHexTwoToStream_Test)
{
    _stream << _packet;

    EXPECT_EQ(0x02, readUInt8(0) & 0x0f);
}

TEST_F(UnsubscribePacketTestWithStream, packetIdentifierWritesToStream_Test)
{
    _packet.setPacketIdentifier(42);
    _stream << _packet;

    EXPECT_EQ(42, readUInt16(variableHeaderOffset()));
}

TEST_F(UnsubscribePacketTestWithStream, topicFilterListWritesToStream_Test)
{
    _packet.setPacketIdentifier(42);
    _packet.setTopicFilterList(QStringList() << "first" << "second");
    _stream << _packet;

    EXPECT_EQ("first", readString(variableHeaderOffset() + 2));
    EXPECT_EQ("second", readString());
}

TEST_F(UnsubscribePacketTestWithStream, packetIdentifierReadsFromStream_Test)
{
    streamIntoPacket(QMQTT::UnsubscribeType << 4, 10, 42, "first");

    EXPECT_EQ(42, _packet.packetIdentifier());
}

TEST_F(UnsubscribePacketTestWithStream, topicFilterListReadsFromStream_Test)
{
    streamIntoPacket(QMQTT::UnsubscribeType << 4, 17, 42, "first", "second");

    EXPECT_EQ(19, _byteArray.size());
    EXPECT_EQ(2, _packet.topicFilterList().size());
    EXPECT_EQ("first", _packet.topicFilterList().at(0));
    EXPECT_EQ("second", _packet.topicFilterList().at(1));
}

TEST_F(UnsubscribePacketTestWithStream, typeUnsubscribeAndFixedHeaderFlagsTwoIsValid_Test)
{
    streamIntoPacket((QMQTT::UnsubscribeType << 4) | 0x02, 17, 42, "first", "second");

    EXPECT_TRUE(_packet.isValid());
}

TEST_F(UnsubscribePacketTestWithStream, typeNotUnsubscribeAndFixedHeaderFlagsTwoIsInvalid_Test)
{
    streamIntoPacket((QMQTT::ConnectType << 4) | 0x02, 17, 42, "first", "second");

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(UnsubscribePacketTestWithStream, typeUnsubscribeAndFixedHeaderFlagsNotTwoIsInvalid_Test)
{
    streamIntoPacket((QMQTT::UnsubscribeType << 4) | 0x01, 17, 42, "first", "second");

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(UnsubscribePacketTestWithStream, emptyTopicFilterListIsInvalid_Test)
{
    streamIntoPacket((QMQTT::UnsubscribeType << 4) | 0x02, 2, 42);

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(UnsubscribePacketTestWithStream, emptyTopicNameIsInvalid_Test)
{
    streamIntoPacket((QMQTT::UnsubscribeType << 4) | 0x02, 10, 42, "");

    EXPECT_FALSE(_packet.isValid());
}
