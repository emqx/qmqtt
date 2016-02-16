#include "basepackettest.h"
#include <qmqtt_connectpacket.h>
#include <QBuffer>
#include <gtest/gtest.h>

using namespace testing;

class ConnectPacketTest : public Test
{
public:
    ConnectPacketTest() {}
    virtual ~ConnectPacketTest() {}

    QMQTT::ConnectPacket _packet;
};

TEST_F(ConnectPacketTest, defaultConstructorValues_Test)
{
    EXPECT_EQ(QMQTT::ConnectType, _packet.type());
    EXPECT_EQ("MQTT", _packet.protocol());
    EXPECT_EQ(0x04, _packet.protocolLevel());
    EXPECT_FALSE(_packet.cleanSession());
    EXPECT_TRUE(_packet.willTopic().isEmpty());
    EXPECT_TRUE(_packet.willMessage().isEmpty());
    EXPECT_EQ(static_cast<quint8>(0), _packet.willQos());
    EXPECT_FALSE(_packet.willRetain());
    EXPECT_TRUE(_packet.clientIdentifier().isEmpty());
    EXPECT_TRUE(_packet.userName().isEmpty());
    EXPECT_TRUE(_packet.password().isEmpty());
}

TEST_F(ConnectPacketTest, type_Test)
{
    EXPECT_EQ(QMQTT::ConnectType, _packet.type());
}

TEST_F(ConnectPacketTest, setCleanSession_Test)
{
    _packet.setCleanSession(true);
    EXPECT_TRUE(_packet.cleanSession());
}

TEST_F(ConnectPacketTest, setWillTopic_Test)
{
    _packet.setWillTopic("topic");
    EXPECT_EQ("topic", _packet.willTopic());
}

TEST_F(ConnectPacketTest, setWillMessage_Test)
{
    _packet.setWillMessage("message");
    EXPECT_EQ("message", _packet.willMessage());
}

TEST_F(ConnectPacketTest, setWillQos_Test)
{
    _packet.setWillQos(2);
    EXPECT_EQ(2, _packet.willQos());
}

TEST_F(ConnectPacketTest, setWillRetain_Test)
{
    _packet.setWillRetain(true);
    EXPECT_TRUE(_packet.willRetain());
}

TEST_F(ConnectPacketTest, setClientIdentifier_Test)
{
    _packet.setClientIdentifier("identifier");
    EXPECT_EQ("identifier", _packet.clientIdentifier());
}

TEST_F(ConnectPacketTest, setUserName_Test)
{
    _packet.setUserName("username");
    EXPECT_EQ("username", _packet.userName());
}

TEST_F(ConnectPacketTest, setPassword_Test)
{
    _packet.setPassword("password");
    EXPECT_EQ("password", _packet.password());
}

class ConnectPacketTestWithStream : public BasePacketTest
{
public:
    ConnectPacketTestWithStream() {}
    virtual ~ConnectPacketTestWithStream() {}

    QMQTT::ConnectPacket _packet;

    void streamIntoPacket(
        const quint8 fixedHeader, const qint64 remainingLength,
        const QString& protocol, const quint8 protocolLevel,
        const quint8 controlFlags, const quint16 keepAlive,
        const QString& clientIdentifier, const QString& willTopic,
        const QString& willMessage, const QString& userName,
        const QString& password)
    {
        _stream << fixedHeader;
        writeRemainingLength(remainingLength);

        writeString(protocol);
        _stream << protocolLevel;
        _stream << controlFlags;
        _stream << keepAlive;
        writeString(clientIdentifier);
        if (!willTopic.isEmpty())
        {
            writeString(willTopic);
        }
        if (!willMessage.isEmpty())
        {
            writeString(willMessage);
        }
        if (!userName.isEmpty())
        {
            writeString(userName);
        }
        if (!password.isEmpty())
        {
            writeString(password);
        }
        _buffer.seek(0);
        _stream >> _packet;
    }

};

// read tests

TEST_F(ConnectPacketTestWithStream, fixedHeaderTypeWritesConnectTypeToStream_Test)
{
    _stream << _packet;

    EXPECT_EQ(QMQTT::ConnectType,
              static_cast<QMQTT::PacketType>((readUInt8(0) & 0xf0) >> 4));
}

TEST_F(ConnectPacketTestWithStream, fixedHeaderFlagsWritesAllBitsFalseToStream_Test)
{
    _stream << _packet;

    EXPECT_EQ(0x00, readUInt8(0) & 0x0f);
}

TEST_F(ConnectPacketTestWithStream, protocolWritesToStream_Test)
{
    _stream << _packet;

    EXPECT_EQ("MQTT", readString(variableHeaderOffset()));
}

TEST_F(ConnectPacketTestWithStream, protocolLevelWritesToStream_Test)
{
    _stream << _packet;

    EXPECT_EQ(4, readUInt8(variableHeaderOffset() + 6));
}

TEST_F(ConnectPacketTestWithStream, connectFlagsWritesToStream_Test)
{
    _stream << _packet;

    EXPECT_EQ(0x00, readUInt8(variableHeaderOffset() + 7));
}

TEST_F(ConnectPacketTestWithStream, keepAliveWritesToStream_Test)
{
    _stream << _packet;

    EXPECT_EQ(300, readUInt16(variableHeaderOffset() + 8));
}

TEST_F(ConnectPacketTestWithStream, clientIdentifierWritesToStream_Test)
{
    _packet.setClientIdentifier("identifier");
    _stream << _packet;

    EXPECT_EQ("identifier", readString(variableHeaderOffset() + 10));
}

TEST_F(ConnectPacketTestWithStream, willTopicWritesToStream_Test)
{
    _packet.setWillTopic("topic");
    _stream << _packet;

    EXPECT_NE(0, readUInt8(variableHeaderOffset() + 7) & 0x04);
    EXPECT_EQ("topic", readString(variableHeaderOffset() + 12));
}

TEST_F(ConnectPacketTestWithStream, willMessageWritesToStream_Test)
{
    _packet.setWillTopic("topic");
    _packet.setWillMessage("message");
    _stream << _packet;

    EXPECT_NE(0, readUInt8(variableHeaderOffset() + 7) & 0x04);
    EXPECT_EQ("message", readString(variableHeaderOffset() + 19));
}

TEST_F(ConnectPacketTestWithStream, willQosWritesToStream_Test)
{
    _packet.setWillQos(2);
    _stream << _packet;

    EXPECT_EQ(2, (readUInt8(variableHeaderOffset() + 7) & 0x18) >> 3);
}

TEST_F(ConnectPacketTestWithStream, willRetainWritesToStream_Test)
{
    _packet.setWillRetain(true);
    _stream << _packet;

    EXPECT_NE(0, readUInt8(variableHeaderOffset() + 7) & 0x20);
}

TEST_F(ConnectPacketTestWithStream, userNameWritesToStream_Test)
{
    _packet.setUserName("username");
    _stream << _packet;

    EXPECT_NE(0, readUInt8(variableHeaderOffset() + 7) & 0x80);
    EXPECT_EQ("username", readString(variableHeaderOffset() + 12));
}

TEST_F(ConnectPacketTestWithStream, passwordWritesToStream_Test)
{
    _packet.setUserName("username");
    _packet.setPassword("password");
    _stream << _packet;

    EXPECT_NE(0, readUInt8(variableHeaderOffset() + 7) & 0x40);
    EXPECT_EQ("password", readString(variableHeaderOffset() + 22));
}

TEST_F(ConnectPacketTestWithStream, protocolReadsFromStream_Test)
{
    streamIntoPacket(QMQTT::ConnectType << 4, 22, "MQTT", 4, 0, 300, "identifier", "", "", "", "");

    EXPECT_EQ("MQTT", _packet.protocol());
}

TEST_F(ConnectPacketTestWithStream, protocolLevelReadsFromStream_Test)
{
    streamIntoPacket(QMQTT::ConnectType << 4, 22, "MQTT", 4, 0, 300, "identifier", "", "", "", "");

    EXPECT_EQ(4, _packet.protocolLevel());
}

TEST_F(ConnectPacketTestWithStream, cleanSessionReadsFromStream_Test)
{
    streamIntoPacket(QMQTT::ConnectType << 4, 22, "MQTT", 4, 0x02, 300, "identifier", "", "", "", "");

    EXPECT_TRUE(_packet.cleanSession());
}

TEST_F(ConnectPacketTestWithStream, willQosReadsFromStream_Test)
{
    streamIntoPacket(QMQTT::ConnectType << 4, 22, "MQTT", 4, 0x10, 300, "identifier", "", "", "", "");

    EXPECT_EQ(2, _packet.willQos());
}

TEST_F(ConnectPacketTestWithStream, willRetainReadsFromStream_Test)
{
    streamIntoPacket((QMQTT::ConnectType << 4) | 0x01, 22, "MQTT", 4, 0x20, 300, "identifier", "", "", "", "");

    EXPECT_TRUE(_packet.willRetain());
}

TEST_F(ConnectPacketTestWithStream, clientIdentifierReadsFromStream_Test)
{
    streamIntoPacket((QMQTT::ConnectType << 4) | 0x01, 22, "MQTT", 4, 0, 300, "identifier", "", "", "", "");

    EXPECT_EQ("identifier", _packet.clientIdentifier());
}

TEST_F(ConnectPacketTestWithStream, willTopicReadsFromStream_Test)
{
    streamIntoPacket((QMQTT::ConnectType << 4) | 0x01, 29, "MQTT", 4, 0x04, 300, "identifier", "topic", "", "", "");

    EXPECT_EQ("topic", _packet.willTopic());
}

TEST_F(ConnectPacketTestWithStream, willMessageReadsFromStream_Test)
{
    streamIntoPacket((QMQTT::ConnectType << 4) | 0x01, 38, "MQTT", 4, 0x04, 300, "identifier", "topic", "message", "", "");

    EXPECT_EQ("message", _packet.willMessage());
}

TEST_F(ConnectPacketTestWithStream, userNameReadsFromStream_Test)
{
    streamIntoPacket((QMQTT::ConnectType << 4) | 0x01, 32, "MQTT", 4, 0x80, 300, "identifier", "", "", "username", "");

    EXPECT_EQ("username", _packet.userName());
}

TEST_F(ConnectPacketTestWithStream, passwordReadsFromStream_Test)
{
    streamIntoPacket((QMQTT::ConnectType << 4) | 0x01, 42, "MQTT", 4, 0xc0, 300, "identifier", "", "", "username", "password");

    EXPECT_EQ("password", _packet.password());
}

// isValid tests

TEST_F(ConnectPacketTestWithStream, typeConnectAndFixedHeaderFlagsZeroIsValid_Test)
{
    streamIntoPacket(QMQTT::ConnectType << 4, 22, "MQTT", 4, 0x00, 300, "identifier", "", "", "", "");

    EXPECT_TRUE(_packet.isValid());
}

TEST_F(ConnectPacketTestWithStream, typeNotConnectAndFixedHeaderFlagsZeroIsInvalid_Test)
{
    streamIntoPacket(QMQTT::ConnackType << 4, 22, "MQTT", 4, 0x00, 300, "identifier", "", "", "", "");

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(ConnectPacketTestWithStream, typeConnectAndFixedHeaderFlagsNotZeroIsInvalid_Test)
{
    streamIntoPacket((QMQTT::ConnackType << 4) | 0x01, 22, "MQTT", 4, 0x00, 300, "identifier", "", "", "", "");

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(ConnectPacketTestWithStream, packetWithClientIdentifierIsValid_Test)
{
    streamIntoPacket(QMQTT::ConnectType << 4, 22, "MQTT", 4, 0x00, 300, "identifier", "", "", "", "");

    EXPECT_TRUE(_packet.isValid());
}

TEST_F(ConnectPacketTestWithStream, packetWithoutClientIdentifierIsInvalid_Test)
{
    streamIntoPacket((QMQTT::ConnectType << 4) | 0x01, 10, "MQTT", 4, 0x00, 300, "", "", "", "", "");

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(ConnectPacketTestWithStream, packetWithoutMQTTProtocolIsInvalid_Test)
{
    streamIntoPacket((QMQTT::ConnectType << 4) | 0x01, 22, "XQTT", 4, 0x00, 300, "identifier", "", "", "", "");

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(ConnectPacketTestWithStream, packetWithoutProtocolLevel4IsInvalid_Test)
{
    streamIntoPacket((QMQTT::ConnectType << 4) | 0x01, 22, "MQTT", 5, 0x00, 300, "identifier", "", "", "", "");

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(ConnectPacketTestWithStream, packetWithControlFlagsReservedBitTrueIsInvalid_Test)
{
    streamIntoPacket(QMQTT::ConnectType << 4, 22, "MQTT", 4, 0x01, 300, "identifier", "", "", "", "");

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(ConnectPacketTestWithStream, packetWithControlFlagsWillFlagTrueAndWillTopicEmptyIsInvalid_Test)
{
    streamIntoPacket((QMQTT::ConnectType << 4) | 0x01, 22, "MQTT", 4, 0x04, 300, "identifier", "", "", "", "");

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(ConnectPacketTestWithStream, packetWithControlFlagsWillFlagTrueAndWillQosGreaterThan2IsInvalid_Test)
{
    streamIntoPacket((QMQTT::ConnectType << 4) | 0x01, 29, "MQTT", 4, 0x1c, 300, "identifier", "topic", "", "", "");

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(ConnectPacketTestWithStream, packetWithControlFlagsWillFlagFalseAndWillQosNotZeroIsInvalid_Test)
{
    streamIntoPacket((QMQTT::ConnectType << 4) | 0x01, 22, "MQTT", 4, 0x08, 300, "identifier", "", "", "", "");

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(ConnectPacketTestWithStream, packetWithControlFlagsWillFlagFalseAndWillRetainTrueIsInvalid_Test)
{
    streamIntoPacket((QMQTT::ConnectType << 4) | 0x01, 22, "MQTT", 4, 0x20, 300, "identifier", "", "", "", "");

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(ConnectPacketTestWithStream, packetWithControlFlagsUserNameFlagFalseAndEmptyUserNameIsInvalid_Test)
{
    streamIntoPacket((QMQTT::ConnectType << 4) | 0x01, 22, "MQTT", 4, 0x80, 300, "identifier", "", "", "", "");

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(ConnectPacketTestWithStream, packetWithControlFlagsUserNameFlagFalseAndPasswordFlagTrueIsInvalid_Test)
{
    streamIntoPacket((QMQTT::ConnectType << 4) | 0x01, 22, "MQTT", 4, 0x40, 300, "identifier", "", "", "", "");

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(ConnectPacketTestWithStream, packetWithControlFlagsPasswordFlagTrueAndPasswordEmptyIsInvalid_Test)
{
    streamIntoPacket((QMQTT::ConnectType << 4) | 0x01, 22, "MQTT", 4, 0xc0, 300, "identifier", "", "", "username", "");

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(ConnectPacketTestWithStream, packetWithInvalidCharacterInClientIdentifierIsInvalid_Test)
{
    streamIntoPacket((QMQTT::ConnectType << 4) | 0x01, 30, "MQTT", 4, 0x00, 300, "identifier-invalid", "", "", "", "");

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(ConnectPacketTestWithStream, packetWithTooLongClientIdentifierIsInvalid_Test)
{
    streamIntoPacket((QMQTT::ConnectType << 4) | 0x01, 36, "MQTT", 4, 0x00, 300, "123456789012345678901234", "", "", "", "");

    EXPECT_FALSE(_packet.isValid());
}
