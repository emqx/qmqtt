#include "basepackettest.h"
#include "testutils.h"
#include <qmqtt_connectpacket.h>
#include <QBuffer>
#include <QDebug>
#include <gtest/gtest.h>

using namespace testing;
using namespace TestUtils;

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
    EXPECT_EQ(300, _packet.keepAlive());
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

TEST_F(ConnectPacketTest, setKeepAlive_Test)
{
    _packet.setKeepAlive(15);
    EXPECT_EQ(15, _packet.keepAlive());
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
    _packet.setWillQos(QMQTT::Qos2);
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

TEST_F(ConnectPacketTest, toFrame_Test)
{
    _packet.setCleanSession(true);
    _packet.setKeepAlive(15);
    _packet.setClientIdentifier("clientId");
    _packet.setWillTopic("topic");
    _packet.setWillQos(QMQTT::Qos2);
    _packet.setWillRetain(true);
    _packet.setWillMessage("message");
    _packet.setUserName("userName");
    _packet.setPassword("password");
    QMQTT::Frame frame = _packet.toFrame();

    EXPECT_EQ(QMQTT::ConnectType << 4, frame._header);
    ASSERT_EQ(56, frame._data.size());
    EXPECT_EQ(0, frame._data.at(0));
    EXPECT_EQ(4, frame._data.at(1));
    EXPECT_EQ("MQTT" , QString::fromUtf8(frame._data.mid(2, 4)));
    EXPECT_EQ(4, frame._data.at(6));
    EXPECT_EQ(0xf6, static_cast<quint8>(frame._data.at(7)));
    EXPECT_EQ(0, frame._data.at(8));
    EXPECT_EQ(15, frame._data.at(9));
    EXPECT_EQ(0, frame._data.at(10));
    EXPECT_EQ(8, frame._data.at(11));
    EXPECT_EQ("clientId", QString::fromUtf8(frame._data.mid(12, 8)));
    EXPECT_EQ(0, frame._data.at(20));
    EXPECT_EQ(5, frame._data.at(21));
    EXPECT_EQ("topic", QString::fromUtf8(frame._data.mid(22, 5)));
    EXPECT_EQ(0, frame._data.at(27));
    EXPECT_EQ(7, frame._data.at(28));
    EXPECT_EQ("message", QString::fromUtf8(frame._data.mid(29, 7)));
    EXPECT_EQ(0, frame._data.at(36));
    EXPECT_EQ(8, frame._data.at(37));
    EXPECT_EQ("userName", QString::fromUtf8(frame._data.mid(38, 8)));
    EXPECT_EQ(0, frame._data.at(46));
    EXPECT_EQ(8, frame._data.at(47));
    EXPECT_EQ("password", QString::fromUtf8(frame._data.mid(48, 8)));
}

TEST_F(ConnectPacketTest, fromFrame_Test)
{
    QMQTT::Frame frame;
    frame._header = QMQTT::ConnectType << 4;
    QBuffer buffer(&frame._data);
    buffer.open(QIODevice::WriteOnly);
    QDataStream stream(&buffer);
    writeString(stream, "MQTT");
    stream << static_cast<quint8>(0x04);
    quint8 controlFlags = 0x00;
    controlFlags |= 1 << 1;
    controlFlags |= 1 << 2;
    controlFlags |= 2 << 3;
    controlFlags |= 1 << 5;
    controlFlags |= 1 << 6;
    controlFlags |= 1 << 7;
    stream << controlFlags;
    stream << static_cast<quint16>(15);
    writeString(stream, "clientId");
    writeString(stream, "topic");
    writeString(stream, "message");
    writeString(stream, "userName");
    writeString(stream, "password");
    buffer.close();

    QMQTT::ConnectPacket packet = QMQTT::ConnectPacket::fromFrame(frame);

    EXPECT_EQ("MQTT", packet.protocol());
    EXPECT_EQ(0x04, packet.protocolLevel());
    EXPECT_TRUE(packet.cleanSession());
    EXPECT_EQ(15, packet.keepAlive());
    EXPECT_EQ("clientId", packet.clientIdentifier());
    EXPECT_EQ("topic", packet.willTopic());
    EXPECT_EQ(2, packet.willQos());
    EXPECT_TRUE(packet.willRetain());
    EXPECT_EQ("message", packet.willMessage());
    EXPECT_EQ("userName", packet.userName());
    EXPECT_EQ("password", packet.password());
}

TEST_F(ConnectPacketTest, defaultPacketWithClientIdIsValid_Test)
{
    _packet.setClientIdentifier("clientId");
    EXPECT_TRUE(_packet.isValid());
}

TEST_F(ConnectPacketTest, controlPacketReservedBitsNotZeroInvalid_Test)
{
    _packet.setClientIdentifier("clientId");
    ASSERT_TRUE(_packet.isValid());

    QMQTT::Frame frame = _packet.toFrame();
    frame._header |= 0x01;
    _packet = QMQTT::ConnectPacket::fromFrame(frame);

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(ConnectPacketTest, supportedProtocolNotMQTTIsInvalid_Test)
{
    _packet.setClientIdentifier("clientId");
    ASSERT_TRUE(_packet.isValid());

    QMQTT::Frame frame = _packet.toFrame();
    frame._data.replace(2, 4, QString("ABCD").toUtf8());
    _packet = QMQTT::ConnectPacket::fromFrame(frame);

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(ConnectPacketTest, supportedProtocolLevelNot4IsInvalid_Test)
{
    _packet.setClientIdentifier("clientId");
    ASSERT_TRUE(_packet.isValid());

    QMQTT::Frame frame = _packet.toFrame();
    frame._data[6] = 0x03;
    _packet = QMQTT::ConnectPacket::fromFrame(frame);

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(ConnectPacketTest, willFlagTrueAndQos3IsInvalid_Test)
{
    _packet.setClientIdentifier("clientId");
    ASSERT_TRUE(_packet.isValid());

    _packet.setWillTopic("topic");
    QMQTT::Frame frame = _packet.toFrame();
    frame._data[7] = frame._data.at(7) | 0x18;
    ASSERT_NE(0, frame._data.at(7) & 0x04);
    ASSERT_EQ(3, (frame._data.at(7) & 0x18) >> 3);
    _packet = QMQTT::ConnectPacket::fromFrame(frame);

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(ConnectPacketTest, willFlagFalseAndQosNotZeroIsInvalid_Test)
{
    _packet.setClientIdentifier("clientId");
    ASSERT_TRUE(_packet.isValid());

    QMQTT::Frame frame = _packet.toFrame();
    frame._data[7] = frame._data.at(7) | 0x08;
    ASSERT_EQ(0, frame._data.at(7) & 0x04);
    ASSERT_EQ(1, (frame._data.at(7) & 0x18) >> 3);
    _packet = QMQTT::ConnectPacket::fromFrame(frame);

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(ConnectPacketTest, willFlagFalseAndWillRetainTrueIsInvalid_Test)
{
    _packet.setClientIdentifier("clientId");
    ASSERT_TRUE(_packet.isValid());

    QMQTT::Frame frame = _packet.toFrame();
    frame._data[7] = frame._data.at(7) | 0x20;
    ASSERT_EQ(0, frame._data.at(7) & 0x04);
    ASSERT_NE(0, frame._data.at(7) & 0x20);
    _packet = QMQTT::ConnectPacket::fromFrame(frame);

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(ConnectPacketTest, passwordWithNoUserNameIsInvalid_Test)
{
    _packet.setClientIdentifier("clientId");
    ASSERT_TRUE(_packet.isValid());

    _packet.setPassword("password");
    ASSERT_FALSE(_packet.password().isEmpty());
    ASSERT_TRUE(_packet.userName().isEmpty());

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(ConnectPacketTest, emptyClientIdentifierIsInvalid_Test)
{
    ASSERT_TRUE(_packet.clientIdentifier().isEmpty());

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(ConnectPacketTest, clientIdentifierWithNonAlphanumbericCharacterIsInvalid_Test)
{
    _packet.setClientIdentifier("clientId");
    ASSERT_TRUE(_packet.isValid());

    _packet.setClientIdentifier("$");

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(ConnectPacketTest, clientIdentifierWith23CharactersValid_Test)
{
    _packet.setClientIdentifier("clientId");
    ASSERT_TRUE(_packet.isValid());

    _packet.setClientIdentifier("12345678901234567890123");

    EXPECT_TRUE(_packet.isValid());
}

TEST_F(ConnectPacketTest, clientIdentifierWith24CharactersInvalid_Test)
{
    _packet.setClientIdentifier("clientId");
    ASSERT_TRUE(_packet.isValid());

    _packet.setClientIdentifier("123456789012345678901234");

    EXPECT_FALSE(_packet.isValid());
}
