#include "socketmock.h"
#include "timermock.h"
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
    NetworkTest() {}
    virtual ~NetworkTest() {}

    void SetUp()
    {
        _byteArray.clear();
        _socketMock = new SocketMock;
        _timerMock = new TimerMock;
        EXPECT_CALL(*_timerMock, setSingleShot(_)).WillRepeatedly(Return());
        EXPECT_CALL(*_timerMock, setInterval(_)).WillRepeatedly(Return());
        _network.reset(new QMQTT::Network(_socketMock, _timerMock));
        Mock::VerifyAndClearExpectations(_socketMock);
    }

    void TearDown()
    {
        _network.reset();
        _byteArray.clear();
    }

    qint64 readDataFromFixtureByteArray(char* data, qint64 requestedLength)
    {
        qint64 actualLength = qMin(requestedLength, static_cast<qint64>(_byteArray.size()));
        if(actualLength > 0)
        {
            memcpy(data, _byteArray.constData(), static_cast<std::size_t>(actualLength));
            _byteArray.remove(0, actualLength);
        }
        return actualLength;
    }

    bool fixtureByteArrayIsEmpty() const
    {
        return _byteArray.isEmpty();
    }

    SocketMock* _socketMock;
    TimerMock* _timerMock;
    QSharedPointer<QMQTT::Network> _network;
    QByteArray _byteArray;
};

TEST(NetworkNoFixtureTest, networkConstructorSetsAutoReconnectTimerSingleShotTrue_Test)
{
    SocketMock* socketMock = new SocketMock;
    TimerMock* timerMock = new TimerMock;
    EXPECT_CALL(*timerMock, setSingleShot(true)).WillOnce(Return());
    EXPECT_CALL(*timerMock, setInterval(_)).WillRepeatedly(Return());
    QMQTT::Network network(socketMock, timerMock);
}

TEST(NetworkNoFixtureTest, networkConstructorSetsAutoReconnectTimerInterval_Test)
{
    SocketMock* socketMock = new SocketMock;
    TimerMock* timerMock = new TimerMock;
    EXPECT_CALL(*timerMock, setSingleShot(_)).WillRepeatedly(Return());
    EXPECT_CALL(*timerMock, setInterval(5000)).WillOnce(Return());
    QMQTT::Network network(socketMock, timerMock);
}

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

TEST_F(NetworkTest, networkConnectToHostCallsSocketConnectToHost_Test)
{
    EXPECT_CALL(*_socketMock, connectToHost(HOST, PORT));
    _network->connectToHost(HOST, PORT);
}

TEST_F(NetworkTest, networkDisconnectFromHostCallsSocketDisconnectFromHost_Test)
{
    EXPECT_CALL(*_socketMock, disconnectFromHost());
    _network->disconnectFromHost();
}

TEST_F(NetworkTest, networkStateCallsSocketState_Test)
{
    EXPECT_CALL(*_socketMock, state()).WillOnce(Return(QAbstractSocket::ConnectedState));
    EXPECT_EQ(QAbstractSocket::ConnectedState, _network->state());
}

TEST_F(NetworkTest, networkAutoReconnectDefaultsToFalse_Test)
{
    EXPECT_FALSE(_network->autoReconnect());
}

TEST_F(NetworkTest, networkSetAutoReconnectTrueSetsAutoReconnectTrue_Test)
{
    _network->setAutoReconnect(true);
    EXPECT_TRUE(_network->autoReconnect());
}

TEST_F(NetworkTest, networkSendFrameWillNotSendAFrameIfNotConnected_Test)
{
    EXPECT_CALL(*_socketMock, state()).WillOnce(Return(QAbstractSocket::UnconnectedState));
    EXPECT_CALL(*_socketMock, writeData(_, _)).Times(0);

    QMQTT::Frame frame;
    _network->sendFrame(frame);
}

TEST_F(NetworkTest, networkSendFrameWillSendAFrameIfConnected_Test)
{
    EXPECT_CALL(*_socketMock, state()).WillOnce(Return(QAbstractSocket::ConnectedState));
    EXPECT_CALL(*_socketMock, writeData(_, _));

    QMQTT::Frame frame;
    _network->sendFrame(frame);
}

TEST_F(NetworkTest, networkEmitsConnectedSignalWhenSocketEmitsConnectedSignal_Test)
{
    QSignalSpy spy(_network.data(), &QMQTT::Network::connected);
    emit _socketMock->connected();
    EXPECT_EQ(1, spy.count());
}

TEST_F(NetworkTest, networkEmitsDisconnectedSignalWhenSocketEmitsDisconnectedSignal_Test)
{
    QSignalSpy spy(_network.data(), &QMQTT::Network::disconnected);
    emit _socketMock->disconnected();
    EXPECT_EQ(1, spy.count());
}

TEST_F(NetworkTest, networkEmitsReceivedSignalOnceAFrameIsReceived_Test)
{
    QByteArray payload(129, 'a');

    QBuffer buffer(&_byteArray);
    buffer.open(QIODevice::WriteOnly);

    QDataStream out(&buffer);
    out << static_cast<quint8>(0x30); // publish header
    out << static_cast<quint8>(0x81); // remaining length most-signficant 7 bits
    out << static_cast<quint8>(0x01); // remaining Length least-significant 7 bits
    buffer.close();
    _byteArray += payload;
    EXPECT_EQ(132, _byteArray.size());

    EXPECT_CALL(*_socketMock, atEnd())
        .WillRepeatedly(Invoke(this, &NetworkTest::fixtureByteArrayIsEmpty));
    EXPECT_CALL(*_socketMock, readData(_, _))
        .WillRepeatedly(Invoke(this, &NetworkTest::readDataFromFixtureByteArray));

    QSignalSpy spy(_network.data(), &QMQTT::Network::received);
    emit _socketMock->readyRead();
    EXPECT_EQ(1, spy.count());
    EXPECT_EQ(payload, spy.at(0).at(0).value<QMQTT::Frame>().data());
}

TEST_F(NetworkTest, autoReconnectWillAttemptToReconnectOnDisconnectionIfAutoReconnectIsTrue_Test)
{
    EXPECT_CALL(*_timerMock, start()).WillRepeatedly(DoAll(
        Invoke(_timerMock, &QMQTT::TimerInterface::timeout),
        Return()));
    _network->setAutoReconnect(true);

    EXPECT_CALL(*_socketMock, connectToHost(_, _));
    emit _socketMock->disconnected();
}

TEST_F(NetworkTest, autoReconnectWillNotAttemptToReconnectOnDisconnectionIfAutoReconnectIsFalse_Test)
{
    EXPECT_CALL(*_timerMock, start()).WillRepeatedly(DoAll(
        Invoke(_timerMock, &QMQTT::TimerInterface::timeout),
        Return()));
    _network->setAutoReconnect(false);

    EXPECT_CALL(*_socketMock, connectToHost(_, _)).Times(0);
    emit _socketMock->disconnected();
}
