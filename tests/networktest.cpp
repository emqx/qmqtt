#include "tcpserver.h"
#include <qmqtt_network.h>
#include <qmqtt_frame.h>
#include <QSignalSpy>
#include <QDebug>
#include <QObject>
#include <QScopedPointer>
#include <QDataStream>
#include <gtest/gtest.h>

using namespace testing;

class NetworkTest : public Test
{
public:
    explicit NetworkTest()
        : _network(new QMQTT::Network)
    {
    }
    virtual ~NetworkTest() {}

    QSharedPointer<QMQTT::Network> _network;

    void flushEvents()
    {
        while (QCoreApplication::hasPendingEvents())
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents);
            QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
        }
    }

    QByteArray serializeFrame(QMQTT::Frame& frame) const
    {
        QByteArray bytes;
        QBuffer buffer(&bytes);
        buffer.open(QIODevice::WriteOnly);
        QDataStream out(&buffer);
        frame.write(out);
        return bytes;
    }
};

TEST_F(NetworkTest, defaultConstructor_Test)
{
    EXPECT_FALSE(_network->isConnectedToHost());
    EXPECT_FALSE(_network->autoReconnect());
    EXPECT_EQ(QAbstractSocket::UnconnectedState, _network->state());
}

TEST_F(NetworkTest, connectToMakesTCPConnection_Test)
{
    TcpServer server;
    _network->connectToHost(server.serverAddress(), server.serverPort());
    bool timedOut = false;
    bool connectionMade = server.waitForNewConnection(5000, &timedOut);

    EXPECT_TRUE(connectionMade);
    EXPECT_FALSE(timedOut);
}

TEST_F(NetworkTest, connectedSignalEmittedAfterConnectionMade_Test)
{
    TcpServer server;
    QSignalSpy spy(_network.data(), &QMQTT::Network::connected);
    _network->connectToHost(server.serverAddress(), server.serverPort());
    EXPECT_EQ(0, spy.count());

    flushEvents();
    EXPECT_EQ(1, spy.count());
}

TEST_F(NetworkTest, isConnectedTrueWhenInConnectedState_Test)
{
    TcpServer server;
    _network->connectToHost(server.serverAddress(), server.serverPort());
    flushEvents();

    EXPECT_TRUE(_network->isConnectedToHost());
}

TEST_F(NetworkTest, isConnectedFalseWhenNotInConnectedState_Test)
{
    EXPECT_FALSE(_network->isConnectedToHost());
}

TEST_F(NetworkTest, disconnectWillDisconnectASocketConnection_Test)
{
    TcpServer server;
    _network->connectToHost(server.serverAddress(), server.serverPort());
    flushEvents();
    EXPECT_TRUE(_network->isConnectedToHost());

    _network->disconnectFromHost();
    flushEvents();

    EXPECT_FALSE(_network->isConnectedToHost());
}

TEST_F(NetworkTest, disconnectedSignalEmittedAfterADisconnection_Test)
{
    TcpServer server;
    _network->connectToHost(server.serverAddress(), server.serverPort());
    flushEvents();
    EXPECT_TRUE(_network->isConnectedToHost());

    QSignalSpy spy(_network.data(), &QMQTT::Network::disconnected);
    EXPECT_EQ(0, spy.count());

    _network->disconnectFromHost();
    flushEvents();

    EXPECT_EQ(1, spy.count());
}

TEST_F(NetworkTest, sendframeSendsTheFrame_Test)
{
    TcpServer server;
    _network->connectToHost(server.serverAddress(), server.serverPort());
    flushEvents();
    EXPECT_TRUE(_network->isConnectedToHost());

    QByteArray data("abc");
    QMQTT::Frame frame(42, data);
    _network->sendFrame(frame);
    flushEvents();

    EXPECT_EQ(serializeFrame(frame), server.data());
}

TEST_F(NetworkTest, receivedReceivesAFrame_Test)
{
    TcpServer server;
    _network->connectToHost(server.serverAddress(), server.serverPort());
    flushEvents();
    EXPECT_TRUE(_network->isConnectedToHost());

    qRegisterMetaType<QMQTT::Frame>("QMQTT::Frame");
    QSignalSpy spy(_network.data(), &QMQTT::Network::received);

    QByteArray data("abc");
    QMQTT::Frame frameSent(42, data);
    QByteArray bytesSent = serializeFrame(frameSent);
    server.socket()->write(bytesSent);

    flushEvents();

    EXPECT_EQ(1, spy.count());
    EXPECT_EQ(frameSent, spy.at(0).at(0).value<QMQTT::Frame>());
}

TEST_F(NetworkTest, stateIsUnconnectedStateBeforeAnyConnectionMade_Test)
{
    EXPECT_EQ(QAbstractSocket::UnconnectedState, _network->state());
}

TEST_F(NetworkTest, stateIsConnectingStateAfterToldToConnectButNotYetConnected_Test)
{
    TcpServer server;
    _network->connectToHost(server.serverAddress(), server.serverPort());
    EXPECT_EQ(QAbstractSocket::ConnectingState, _network->state());
}

TEST_F(NetworkTest, stateIsConnectedStateAfterConnectionHasBeenMade_Test)
{
    TcpServer server;
    _network->connectToHost(server.serverAddress(), server.serverPort());
    flushEvents();
    EXPECT_EQ(QAbstractSocket::ConnectedState, _network->state());
}

TEST_F(NetworkTest, stateIsUnconnectedStateAfterGivenDisconnect_Test)
{
    TcpServer server;
    _network->connectToHost(server.serverAddress(), server.serverPort());
    flushEvents();
    EXPECT_EQ(QAbstractSocket::ConnectedState, _network->state());

    _network->disconnectFromHost();
    EXPECT_EQ(QAbstractSocket::UnconnectedState, _network->state());
}

TEST_F(NetworkTest, autoReconnectDefaultsToFalse_Test)
{
    EXPECT_FALSE(_network->autoReconnect());
}

TEST_F(NetworkTest, autoReconnectTrueAfterSetAutoReconnectTrue_Test)
{
    _network->setAutoReconnect(true);
    EXPECT_TRUE(_network->autoReconnect());
}

TEST_F(NetworkTest, willNotAutoReconnectIfAutoReconnectIsSetFalse_Test)
{
    TcpServer server;
    _network->connectToHost(server.serverAddress(), server.serverPort());
    flushEvents();
    EXPECT_TRUE(_network->isConnectedToHost());

    server.socket()->disconnectFromHost();
    server.socket()->state() == QAbstractSocket::UnconnectedState
       || server.socket()->waitForDisconnected(5000);
    flushEvents();

    EXPECT_FALSE(_network->isConnectedToHost());
}

// todo: Autoreconnect feature not yet enabled
TEST_F(NetworkTest, DISABLED_willAutoReconnectIfAutoReconnectIsSetTrue_Test)
{
    _network->setAutoReconnect(true);

    TcpServer server;
    _network->connectToHost(server.serverAddress(), server.serverPort());
    flushEvents();
    EXPECT_TRUE(_network->isConnectedToHost());

    server.socket()->disconnectFromHost();
    server.socket()->waitForDisconnected(5000);
    flushEvents();

    EXPECT_TRUE(_network->autoReconnect());
    EXPECT_TRUE(_network->isConnectedToHost());
}
