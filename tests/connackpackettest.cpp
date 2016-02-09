#include "basepackettest.h"
#include <qmqtt_connackpacket.h>
#include <QBuffer>
#include <QDebug>
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
    EXPECT_FALSE(_packet.sessionPresent());
    EXPECT_EQ(0, _packet.connectReturnCode());
}

TEST_F(ConnackPacketTest, setSessionPresent_Test)
{
    _packet.setSessionPresent(true);
    EXPECT_TRUE(_packet.sessionPresent());
}

TEST_F(ConnackPacketTest, setConnectReturnCode_Test)
{
    _packet.setConnectReturnCode(QMQTT::ConnackPacket::ConnectionRefusedBadUserNameOrPassword);
    EXPECT_EQ(QMQTT::ConnackPacket::ConnectionRefusedBadUserNameOrPassword, _packet.connectReturnCode());
}

class ConnackPacketTestWithStream : public BasePacketTest
{
public:
    ConnackPacketTestWithStream() {}
    virtual ~ConnackPacketTestWithStream() {}

    QMQTT::ConnackPacket _packet;

    void streamIntoPacket(const quint8 connectAcknowledgeFlags,
                          const quint8 connectReturnCode)
    {
        _stream << connectAcknowledgeFlags;
        _stream << connectReturnCode;
        _buffer.seek(0);
        _stream >> _packet;
    }
};

TEST_F(ConnackPacketTestWithStream, fixedHeaderTypeWritesConnectTypeToStream_Test)
{
    _stream << _packet;

    EXPECT_EQ(QMQTT::ConnackType,
              static_cast<QMQTT::PacketType>((readUInt8(0) & 0xf0) >> 4));
}

TEST_F(ConnackPacketTestWithStream, fixedHeaderFlagsWritesAllBitsFalseToStream_Test)
{
    _stream << _packet;

    EXPECT_EQ(0x00, readUInt8(0) & 0x0f);
}

TEST_F(ConnackPacketTestWithStream, connectSessionPresentWritesToStream_Test)
{
    _packet.setSessionPresent(true);
    _stream << _packet;

    EXPECT_EQ(0x01, readUInt8(variableHeaderOffset()));
}

TEST_F(ConnackPacketTestWithStream, connectReturnCodeWritesToStream_Test)
{
    _packet.setConnectReturnCode(QMQTT::ConnackPacket::ConnectionRefusedServerUnavailable);
    _stream << _packet;

    EXPECT_EQ(QMQTT::ConnackPacket::ConnectionRefusedServerUnavailable,
              static_cast<int>(readUInt8(variableHeaderOffset() + 1)));
}

/*
TEST_F(ConnackPacketTestWithStream, connectSessionPresentFromStream_Test)
{
    streamIntoPacket(0x01, QMQTT::ConnackPacket::ConnectionAccepted);

    EXPECT_TRUE(_packet.sessionPresent());
}

TEST_F(ConnackPacketTestWithStream, connectReturnCodeReadsFromStream_Test)
{
    streamIntoPacket(0x00, QMQTT::ConnackPacket::ConnectionRefusedServerUnavailable);

    EXPECT_EQ(QMQTT::ConnackPacket::ConnectionRefusedServerUnavailable, _packet.connectReturnCode());
}

TEST_F(ConnackPacketTestWithStream, connectAcknowledgeFlagsWithReservedBitTrueIsInvalid_Test)
{
    streamIntoPacket(0x02, QMQTT::ConnackPacket::ConnectionAccepted);

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(ConnackPacketTestWithStream, sessionPresentFalseAndConnectReturnCodeNotZeroIsInvalid_Test)
{
    streamIntoPacket(0x00, QMQTT::ConnackPacket::ConnectionRefusedBadUserNameOrPassword);

    EXPECT_FALSE(_packet.isValid());
}
*/
