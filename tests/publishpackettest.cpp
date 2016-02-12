#include "basepackettest.h"
#include <qmqtt_publishpacket.h>
#include <QByteArray>
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
}

TEST_F(PublishPacketTest, setDupFlag_Test)
{
    _packet.setDupFlag(true);
    EXPECT_TRUE(_packet.dupFlag());
}

TEST_F(PublishPacketTest, setQos_Test)
{
    _packet.setQos(2);
    EXPECT_EQ(2, _packet.qos());
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
    _packet.setPayload(QStringLiteral("payload").toUtf8());
    EXPECT_EQ(QStringLiteral("payload"), QString::fromUtf8(_packet.payload()));
}

class PublishPacketTestWithStream : public BasePacketTest
{
public:
    PublishPacketTestWithStream() {}
    virtual ~PublishPacketTestWithStream() {}

    QMQTT::PublishPacket _packet;

    void streamIntoPacket(
        const quint8 fixedHeader,
        const qint64 remainingLength,
        const QString& topicName,
        const quint16 packetIdentifier,
        const QByteArray& payload)
    {
        _stream << fixedHeader;
        writeRemainingLength(remainingLength);
        writeString(topicName);
        if ((fixedHeader & 0x06) != 0)
        {
            _stream << packetIdentifier;
        }
        _stream.writeRawData(payload.constData(), payload.size());
        _buffer.seek(0);
        _stream >> _packet;
    }
};

TEST_F(PublishPacketTestWithStream, fixedHeaderTypeWritesConnectTypeToStream_Test)
{
    _stream << _packet;

    EXPECT_EQ(QMQTT::PublishType,
              static_cast<QMQTT::PacketType>((readUInt8(0) & 0xf0) >> 4));
}

TEST_F(PublishPacketTestWithStream, fixedHeaderFlagsWritesDupFlagFalseByDefaultToStream_Test)
{
    _stream << _packet;

    EXPECT_EQ(0, readUInt8(0) & 0x08);
}

TEST_F(PublishPacketTestWithStream, fixedHeaderFlagsWritesDupFlagTrueWhenDupFlagTrueToStream_Test)
{
    _packet.setDupFlag(true);
    _stream << _packet;

    EXPECT_NE(0, readUInt8(0) & 0x08);
}

TEST_F(PublishPacketTestWithStream, fixedHeaderFlagsWritesQosZeroByDefaultToStream_Test)
{
    _stream << _packet;

    EXPECT_EQ(0, (readUInt8(0) & 0x06) >> 1);
}

TEST_F(PublishPacketTestWithStream, fixedHeaderFlagsWritesQosTwoWhenQosTwoToStream_Test)
{
    _packet.setQos(2);
    _stream << _packet;

    EXPECT_EQ(2, (readUInt8(0) & 0x06) >> 1);
}

TEST_F(PublishPacketTestWithStream, fixedHeaderFlagsWritesRetainFlagFalseByDefaultToStream_Test)
{
    _stream << _packet;

    EXPECT_EQ(0, readUInt8(0) & 0x01);
}

TEST_F(PublishPacketTestWithStream, fixedHeaderFlagsWritesRetainFlagTrueWhenRetainFlagTrueToStream_Test)
{
    _packet.setRetainFlag(true);
    _stream << _packet;

    EXPECT_NE(0, readUInt8(0) & 0x01);
}

TEST_F(PublishPacketTestWithStream, topicNameWritesToStream_Test)
{
    _packet.setTopicName("topic");
    _stream << _packet;

    EXPECT_EQ("topic", readString(variableHeaderOffset()));
}

TEST_F(PublishPacketTestWithStream, packetIdentifierWritesToStream_Test)
{
    _packet.setQos(1);
    _packet.setPacketIdentifier(42);
    _stream << _packet;

    quint16 topicLength = readUInt16(variableHeaderOffset());
    EXPECT_EQ(42, readUInt16(variableHeaderOffset() + topicLength + 2));
}

TEST_F(PublishPacketTestWithStream, payloadWritesToStream_Test)
{
    _packet.setPayload(QStringLiteral("payload").toUtf8());
    _stream << _packet;

    qint64 payloadLength = readRemainingLength() - _packet.topicName().size() - 2;
    EXPECT_EQ("payload",
              readByteArray(variableHeaderOffset() + _packet.topicName().size() + 2, payloadLength));
}

TEST_F(PublishPacketTestWithStream, fixedHeaderFlagsReadsDupFlagTrueFromStream_Test)
{
    streamIntoPacket(0x3a, 16, "topic", 0, QString("payload").toUtf8());

    EXPECT_TRUE(_packet.dupFlag());
}

TEST_F(PublishPacketTestWithStream, fixedHeaderFlagsReadsQosFromStream_Test)
{
    streamIntoPacket(0x34, 16, "topic", 0, QString("payload").toUtf8());

    EXPECT_EQ(2, _packet.qos());
}

TEST_F(PublishPacketTestWithStream, fixedHeaderFlagsReadsRetainFlagFromStream_Test)
{
    streamIntoPacket(0x31, 16, "topic", 0, QString("payload").toUtf8());

    EXPECT_EQ(1, _packet.retainFlag());
}

TEST_F(PublishPacketTestWithStream, fixedHeaderFlagsReadsTopicNameFromStream_Test)
{
    streamIntoPacket(0x30, 16, "topic", 0, QString("payload").toUtf8());

    EXPECT_EQ("topic", _packet.topicName());
}

TEST_F(PublishPacketTestWithStream, fixedHeaderFlagsReadsPacketIdentifierFromStream_Test)
{
    streamIntoPacket(0x32, 18, "topic", 42, QString("payload").toUtf8());

    EXPECT_EQ(42, _packet.packetIdentifier());
}

TEST_F(PublishPacketTestWithStream, fixedHeaderFlagsReadsPayloadFromStream_Test)
{
    streamIntoPacket(0x30, 16, "topic", 0, QString("payload").toUtf8());

    EXPECT_EQ("payload", _packet.payload());
}

TEST_F(PublishPacketTestWithStream, qosZeroWithDupFlagTrueIsInvalid_Test)
{
    streamIntoPacket(0x38, 0, "topic", 0, QString("payload").toUtf8());

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(PublishPacketTestWithStream, qosThreeIsInvalid_Test)
{
    streamIntoPacket(0x36, 0, "topic", 0, QString("payload").toUtf8());

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(PublishPacketTestWithStream, topicNameWithAsteriskIsInvalid_Test)
{
    streamIntoPacket(0x30, 0, "topic/*", 0, QString("payload").toUtf8());

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(PublishPacketTestWithStream, topicNameWithQuestionMarkIsInvalid_Test)
{
    streamIntoPacket(0x30, 0, "topic?", 0, QString("payload").toUtf8());

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(PublishPacketTestWithStream, qosOneAndPacketIdentifierNotZeroIsInvalid_Test)
{
    streamIntoPacket(0x32, 0, "topic", 42, QString("payload").toUtf8());

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(PublishPacketTestWithStream, qosTwoAndPacketIdentifierNotZeroIsInvalid_Test)
{
    streamIntoPacket(0x34, 0, "topic", 42, QString("payload").toUtf8());

    EXPECT_FALSE(_packet.isValid());
}
