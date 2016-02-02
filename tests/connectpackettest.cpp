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

// write to stream

class ConnectPacketTestWithStream : public Test
{
public:
    ConnectPacketTestWithStream()
    {
        _buffer.setBuffer(&_byteArray);
        _buffer.open(QIODevice::ReadWrite);
        _stream.setDevice(&_buffer);
    }
    virtual ~ConnectPacketTestWithStream() {}

    QMQTT::ConnectPacket _packet;
    QByteArray _byteArray;
    QBuffer _buffer;
    QDataStream _stream;

    quint8 readUInt8(int offset)
    {
        quint8 i = 0;
        _buffer.seek(offset);
        _stream >> i;
        return i;
    }

    quint16 readUInt16(int offset)
    {
        quint16 i = 0;
        _buffer.seek(offset);
        _stream >> i;
        return i;
    }

    QString readString(int offset)
    {
        quint16 length = readUInt16(offset);
        QByteArray byteArray;
        byteArray.resize(length);
        _stream.readRawData(byteArray.data(), length);
        return QString::fromUtf8(byteArray);
    }

    void writeString(const QString& string)
    {
        _stream << static_cast<quint16>(string.size());
        _stream.writeRawData(string.toUtf8().constData(), string.size());
    }

    void createByteArray(const QString& protocol, const quint8 protocolLevel,
                         const quint8 controlFlags, const quint16 keepAlive,
                         const QString& clientIdentifier,
                         const QString& userName = QString(),
                         const QString& password = QString())
    {
        writeString(protocol);
        _stream << protocolLevel;
        _stream << controlFlags;
        _stream << keepAlive;
        writeString(clientIdentifier);
        if (!userName.isEmpty())
        {
            writeString(userName);
        }
        if (!password.isEmpty())
        {
            writeString(password);
        }
        _buffer.seek(0);
    }
};

TEST_F(ConnectPacketTestWithStream, protocolWritesToStream_Test)
{
    _stream << _packet;

    EXPECT_EQ("MQTT", readString(0));
}

TEST_F(ConnectPacketTestWithStream, protocolLevelWritesToStream_Test)
{
    _stream << _packet;

    EXPECT_EQ(4, readUInt8(6));
}

TEST_F(ConnectPacketTestWithStream, connectFlagsWritesToStream_Test)
{
    _stream << _packet;

    EXPECT_EQ(0x00, readUInt8(7));
}

TEST_F(ConnectPacketTestWithStream, keepAliveWritesToStream_Test)
{
    _stream << _packet;

    EXPECT_EQ(300, readUInt16(8));
}

TEST_F(ConnectPacketTestWithStream, clientIdentifierWritesToStream_Test)
{
    _packet.setClientIdentifier("identifier");
    _stream << _packet;

    EXPECT_EQ("identifier", readString(10));
}

TEST_F(ConnectPacketTestWithStream, willTopicWritesToStream_Test)
{
    _packet.setWillTopic("topic");
    _stream << _packet;

    EXPECT_NE(0, readUInt8(7)& 0x04);
    EXPECT_EQ("topic", readString(12));
}

TEST_F(ConnectPacketTestWithStream, willMessageWritesToStream_Test)
{
    _packet.setWillTopic("topic");
    _packet.setWillMessage("message");
    _stream << _packet;

    EXPECT_NE(0, readUInt8(7)& 0x04);
    EXPECT_EQ("message", readString(19));
}

TEST_F(ConnectPacketTestWithStream, willQosWritesToStream_Test)
{
    _packet.setWillQos(2);
    _stream << _packet;

    EXPECT_EQ(2, (readUInt8(7) & 0x18) >> 3);
}

TEST_F(ConnectPacketTestWithStream, willRetainWritesToStream_Test)
{
    _packet.setWillRetain(true);
    _stream << _packet;

    EXPECT_NE(0, readUInt8(7) & 0x20);
}

TEST_F(ConnectPacketTestWithStream, userNameWritesToStream_Test)
{
    _packet.setUserName("username");
    _stream << _packet;

    EXPECT_NE(0, readUInt8(7) & 0x80);
    EXPECT_EQ("username", readString(12));
}

TEST_F(ConnectPacketTestWithStream, passwordWritesToStream_Test)
{
    _packet.setUserName("username");
    _packet.setPassword("password");
    _stream << _packet;

    EXPECT_NE(0, readUInt8(7) & 0x40);
    EXPECT_EQ("password", readString(22));
}

// read from stream

TEST_F(ConnectPacketTestWithStream, protocolReadsFromStream_Test)
{
    createByteArray("MQTT", 4, 0, 300, "identifier");
    _stream >> _packet;

    EXPECT_EQ("MQTT", _packet.protocol());
}

TEST_F(ConnectPacketTestWithStream, protocolLevelReadsFromStream_Test)
{
    createByteArray("MQTT", 4, 0, 300, "identifier");
    _stream >> _packet;

    EXPECT_EQ(4, _packet.protocolLevel());
}

TEST_F(ConnectPacketTestWithStream, cleanSessionReadsFromStream_Test)
{
    createByteArray("MQTT", 4, 0x02, 300, "identifier");
    _stream >> _packet;

    EXPECT_TRUE(_packet.cleanSession());
}

TEST_F(ConnectPacketTestWithStream, willQosReadsFromStream_Test)
{
    createByteArray("MQTT", 4, 0x10, 300, "identifier");
    _stream >> _packet;

    EXPECT_EQ(2, _packet.willQos());
}

TEST_F(ConnectPacketTestWithStream, willRetainReadsFromStream_Test)
{
    createByteArray("MQTT", 4, 0x20, 300, "identifier");
    _stream >> _packet;

    EXPECT_TRUE(_packet.willRetain());
}

TEST_F(ConnectPacketTestWithStream, clientIdentifierReadsFromStream_Test)
{
    createByteArray("MQTT", 4, 0, 300, "identifier");
    _stream >> _packet;

    EXPECT_EQ("identifier", _packet.clientIdentifier());
}

TEST_F(ConnectPacketTestWithStream, userNameReadsFromStream_Test)
{
    createByteArray("MQTT", 4, 0x80, 300, "identifier", "username");
    _stream >> _packet;

    EXPECT_EQ("username", _packet.userName());
}

TEST_F(ConnectPacketTestWithStream, passwordReadsFromStream_Test)
{
    createByteArray("MQTT", 4, 0xc0, 300, "identifier", "username", "password");
    _stream >> _packet;

    EXPECT_EQ("password", _packet.password());
}
