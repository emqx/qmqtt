#include "basepackettest.h"
#include <qmqtt_pingreqpacket.h>
#include <gtest/gtest.h>

using namespace testing;

class PingreqPacketTest : public Test
{
public:
    PingreqPacketTest() {}
    virtual ~PingreqPacketTest() {}

    QMQTT::PingreqPacket _packet;
};

TEST_F(PingreqPacketTest, defaultConstructorValues_Test)
{
    EXPECT_EQ(QMQTT::PingreqType, _packet.type());
}

class PingreqPacketTestWithStream : public BasePacketTest
{
public:
    PingreqPacketTestWithStream() {}
    virtual ~PingreqPacketTestWithStream() {}

    QMQTT::PingreqPacket _packet;

    void streamIntoPacket(
        const quint8 fixedHeader,
        const qint64 remainingLength)
    {
        _stream << fixedHeader;
        writeRemainingLength(remainingLength);
        _buffer.seek(0);
        _stream >> _packet;
    }
};

TEST_F(PingreqPacketTestWithStream, fixedHeaderTypeWritesPingreqTypeToStream_Test)
{
    _stream << _packet;

    EXPECT_EQ(QMQTT::PingreqType,
              static_cast<QMQTT::PacketType>((readUInt8(0) & 0xf0) >> 4));
}

TEST_F(PingreqPacketTestWithStream, typePingreqAndFixedHeaderFlagsZeroIsValid_Test)
{
    streamIntoPacket(QMQTT::PingreqType << 4, 0);

    EXPECT_TRUE(_packet.isValid());
}

TEST_F(PingreqPacketTestWithStream, typeNotPingreqAndFixedHeaderFlagsZeroIsInvalid_Test)
{
    streamIntoPacket(QMQTT::ConnectType << 4, 0);

    EXPECT_FALSE(_packet.isValid());
}

TEST_F(PingreqPacketTestWithStream, typePingreqAndFixedHeaderFlagsNotZeroIsInvalid_Test)
{
    streamIntoPacket((QMQTT::PingreqType << 4) | 0x01, 0);

    EXPECT_FALSE(_packet.isValid());
}

