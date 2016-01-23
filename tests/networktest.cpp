#include "socketmock.h"
#include <qmqtt_network.h>
#include <QSignalSpy>
#include <QDataStream>
#include <QCoreApplication>
#include <gtest/gtest.h>

using namespace testing;

const QHostAddress HOST = QHostAddress::LocalHost;
const quint16 PORT = 3875;
const QIODevice::OpenMode OPEN_MODE = QIODevice::ReadWrite;

class NetworkTest : public Test
{
public:
    explicit NetworkTest()
        : _socketMock(new SocketMock)
        , _network(new QMQTT::Network(_socketMock))
    {
    }
    virtual ~NetworkTest() {}

    SocketMock* _socketMock;
    QSharedPointer<QMQTT::Network> _network;
};

TEST_F(NetworkTest, networkIsConnectedReturnsFalseWhenSocketStateIsUnconnectedState_Test)
{
    EXPECT_CALL(*_socketMock, state()).WillOnce(Return(QAbstractSocket::UnconnectedState));
    EXPECT_FALSE(_network->isConnectedToHost());
}

TEST_F(NetworkTest, networkIsConnectedReturnsTrueWhenSocketStateIsConnectedState_Test)
{
    EXPECT_CALL(*_socketMock, state()).WillOnce(Return(QAbstractSocket::ConnectedState));
    EXPECT_TRUE(_network->isConnectedToHost());
}

TEST_F(NetworkTest, networkStateReturnsUnconnectedStateWhenSocketStateIsUnconnectedState_Test)
{
    EXPECT_CALL(*_socketMock, state()).WillOnce(Return(QAbstractSocket::UnconnectedState));
    EXPECT_EQ(QAbstractSocket::UnconnectedState, _network->state());
}

TEST_F(NetworkTest, networkStateReturnsConnectedStateWhenSocketStateIsConnectedState_Test)
{
    EXPECT_CALL(*_socketMock, state()).WillOnce(Return(QAbstractSocket::ConnectedState));
    EXPECT_EQ(QAbstractSocket::ConnectedState, _network->state());
}

TEST_F(NetworkTest, networkConnectToHostCallsSocketConnectToHost)
{
    EXPECT_CALL(*_socketMock, connectToHost(HOST, PORT));
    _network->connectToHost(HOST, PORT);
}

TEST_F(NetworkTest, networkDisconnectFromHostCallsSocketDisconnectFromHost)
{
    EXPECT_CALL(*_socketMock, disconnectFromHost());
    _network->disconnectFromHost();
}

TEST_F(NetworkTest, networkStateCallsSocketState)
{
    EXPECT_CALL(*_socketMock, state()).WillOnce(Return(QAbstractSocket::ConnectedState));
    EXPECT_EQ(QAbstractSocket::ConnectedState, _network->state());
}

TEST_F(NetworkTest, networkAutoReconnectDefaultsToFalse)
{
    EXPECT_FALSE(_network->autoReconnect());
}

TEST_F(NetworkTest, networkSetAutoReconnectTrueSetsAutoReconnectTrue_Test)
{
    _network->setAutoReconnect(true);
    EXPECT_TRUE(_network->autoReconnect());
}

TEST_F(NetworkTest, networkSendFrameWillNotSendAFrameIfNotConnected)
{
    EXPECT_CALL(*_socketMock, state()).WillOnce(Return(QAbstractSocket::UnconnectedState));
    EXPECT_CALL(*_socketMock, writeData(_, _)).Times(0);

    QMQTT::Frame frame;
    _network->sendFrame(frame);
}

TEST_F(NetworkTest, networkSendFrameWillSendAFrameIfConnected)
{
    EXPECT_CALL(*_socketMock, state()).WillOnce(Return(QAbstractSocket::ConnectedState));
    EXPECT_CALL(*_socketMock, writeData(_, _));

    QMQTT::Frame frame;
    _network->sendFrame(frame);
}

TEST_F(NetworkTest, networkEmitsConnectedSignalWhenSocketEmitsConnectedSignal)
{
    QSignalSpy spy(_network.data(), &QMQTT::Network::connected);
    emit _socketMock->connected();
    EXPECT_EQ(1, spy.count());
}

TEST_F(NetworkTest, networkEmitsDisconnectedSignalWhenSocketEmitsDisconnectedSignal)
{
    QSignalSpy spy(_network.data(), &QMQTT::Network::disconnected);
    emit _socketMock->disconnected();
    EXPECT_EQ(1, spy.count());
}

TEST_F(NetworkTest, networkEmitsReceivedSignalOnceAFrameIsReceived)
{
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);

    QDataStream out(&buffer);
    out << static_cast<quint8>(0x30); // publish header
    out << static_cast<quint8>(0x81); // remaining length most-signficant 7 bits
    out << static_cast<quint8>(0x01); // remaining Length least-significant 7 bits
    for (int i = 0; i < 129; ++i)
    {
        out << static_cast<quint8>('a');
    }
    buffer.close();

    EXPECT_CALL(*_socketMock, atEnd()).WillOnce(Return(true));
    EXPECT_CALL(*_socketMock, atEnd()).WillOnce(Return(false)).RetiresOnSaturation();
    EXPECT_CALL(*_socketMock, readData(_, _))
        .WillOnce(DoAll(SetArgPointee<0>(*byteArray.data()), Return(0x84)));

    QSignalSpy spy(_network.data(), &QMQTT::Network::received);
    emit _socketMock->readyRead();
    EXPECT_EQ(1, spy.count());
}
