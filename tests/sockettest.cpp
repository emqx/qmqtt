#include "qmqtt_socket.h"
#include "tcpserver.h"
#include <QCoreApplication>
#include <QSignalSpy>
#include <QSharedPointer>
#include <QDataStream>
#include <QTcpSocket>
#include <gmock/gmock.h>

using namespace testing;

const QHostAddress HOST = QHostAddress::LocalHost;
const quint16 PORT = 3875;
const QByteArray BYTE_ARRAY = QString::fromLatin1("Supercalifragilisticexpialidocious").toUtf8();

class SocketTest : public Test
{
public:
    explicit SocketTest()
        : _socket(new Socket)
    {
    }
    virtual ~SocketTest() {}

    QScopedPointer<Socket> _socket;

    void flushEvents()
    {
        while (QCoreApplication::hasPendingEvents())
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents);
            QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
        }
    }

    QSharedPointer<TcpServer> createAndConnectToServer()
    {
        QSharedPointer<TcpServer> server = QSharedPointer<TcpServer>(new TcpServer(HOST, PORT));
        _socket->connectToHost(server->serverAddress(), server->serverPort());
        bool timedOut = false;
        bool connectionMade = server->waitForNewConnection(5000, &timedOut);
        EXPECT_TRUE(connectionMade);
        EXPECT_FALSE(timedOut);
        EXPECT_THAT(server->socket(), NotNull());
        flushEvents();
        EXPECT_EQ(QAbstractSocket::ConnectedState, server->socket()->state());
        EXPECT_EQ(QAbstractSocket::ConnectedState, _socket->state());
        return server;
    }
};

TEST_F(SocketTest, defaultConstructor_Test)
{
    EXPECT_FALSE(_socket.isNull());
    EXPECT_EQ(QAbstractSocket::UnconnectedState, _socket->state());
}

TEST_F(SocketTest, connectToHostMakesTcpConnection_Test)
{
    TcpServer server(HOST, PORT);
    _socket->connectToHost(server.serverAddress(), server.serverPort());
    bool timedOut = false;
    bool connectionMade = server.waitForNewConnection(5000, &timedOut);

    EXPECT_TRUE(connectionMade);
    EXPECT_FALSE(timedOut);
}

TEST_F(SocketTest, disconnectFromHostDisconnectsTcpConnection_Test)
{
    QSharedPointer<TcpServer> server = createAndConnectToServer();
    QSignalSpy spy(server->socket(), &QTcpSocket::disconnected);

    _socket->disconnectFromHost();
    flushEvents();

    EXPECT_EQ(1, spy.count());
}

TEST_F(SocketTest, tcpConnectionEmitsConnectedSignal_Test)
{
    QSignalSpy spy(_socket.data(), &Socket::connected);

    createAndConnectToServer();
    flushEvents();

    EXPECT_EQ(1, spy.count());
}

TEST_F(SocketTest, tcpDisconnectionEmitsDisconnectedSignal_Test)
{
    createAndConnectToServer();
    QSignalSpy spy(_socket.data(), &Socket::disconnected);

    _socket->disconnectFromHost();
    flushEvents();

    EXPECT_EQ(1, spy.count());
}

TEST_F(SocketTest, remoteDisconnectionEmitsDisconnectedSignal_Test)
{
    QSharedPointer<TcpServer> server = createAndConnectToServer();
    QSignalSpy spy(_socket.data(), &Socket::disconnected);

    server->socket()->disconnectFromHost();
    flushEvents();

    EXPECT_EQ(1, spy.count());
}

TEST_F(SocketTest, connectedSocketShowsConnectedState_Test)
{
    QSharedPointer<TcpServer> server = createAndConnectToServer();
    flushEvents();
    EXPECT_EQ(QAbstractSocket::ConnectedState, _socket->state());
}

TEST_F(SocketTest, incomingDataTriggersReadyReadSignal_Test)
{
    QSharedPointer<TcpServer> server = createAndConnectToServer();
    QSignalSpy spy(_socket.data(), &Socket::readyRead);

    server->socket()->write(BYTE_ARRAY);
    flushEvents();

    EXPECT_EQ(1, spy.count());
}

TEST_F(SocketTest, pendingDataOnSocketReturnsAtEndFalse_Test)
{
    QSharedPointer<TcpServer> server = createAndConnectToServer();
    server->socket()->write(BYTE_ARRAY);
    server->socket()->waitForBytesWritten(5000);
    flushEvents();

    EXPECT_FALSE(_socket->atEnd());
}

TEST_F(SocketTest, noPendingDataOnSocketReturnsAtEndTrue_Test)
{
    QSharedPointer<TcpServer> server = createAndConnectToServer();
    flushEvents();

    EXPECT_TRUE(_socket->atEnd());
}

TEST_F(SocketTest, incomingDataIsRetrivable_Test)
{
    QSharedPointer<TcpServer> server = createAndConnectToServer();
    QSignalSpy spy(_socket.data(), &Socket::readyRead);

    server->socket()->write(BYTE_ARRAY);
    flushEvents();

    EXPECT_EQ(1, spy.count());
    EXPECT_EQ(BYTE_ARRAY, _socket->readAll());
}

TEST_F(SocketTest, outgoingDataIsSent_Test)
{
    QSharedPointer<TcpServer> server = createAndConnectToServer();
    _socket->write(BYTE_ARRAY.data(), BYTE_ARRAY.size());
    EXPECT_TRUE(_socket->waitForBytesWritten(5000));
    EXPECT_TRUE(server->socket()->waitForReadyRead(5000));

    EXPECT_EQ(BYTE_ARRAY, server->data());
}

TEST_F(SocketTest, socketErrorEmitsErrorSignal_Test)
{
    QSignalSpy spy(_socket.data(), static_cast<void (Socket::*)(QAbstractSocket::SocketError)>(&Socket::error));
    _socket->connectToHost(HOST, PORT);
    flushEvents();

    EXPECT_EQ(1, spy.count());
    EXPECT_EQ(QAbstractSocket::ConnectionRefusedError, spy.at(0).at(0).value<QAbstractSocket::SocketError>());
}

TEST_F(SocketTest, errorGivesSocketErrorReason_Test)
{
    _socket->connectToHost(HOST, PORT);
    flushEvents();

    EXPECT_EQ(QAbstractSocket::ConnectionRefusedError, _socket->error());
}
