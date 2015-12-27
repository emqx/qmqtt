#include "tcpserver.h"
#include <qmqtt_network.h>
#include <qmqtt_frame.h>
#include <QTest>
#include <QSignalSpy>
#include <QDebug>
#include <QObject>
#include <QScopedPointer>

class NetworkTests : public QObject
{
    Q_OBJECT
public:
    explicit NetworkTests();
    virtual ~NetworkTests();

    QScopedPointer<QMQTT::Network> _uut;

private:
    void flushEvents();
    QByteArray serializeFrame(QMQTT::Frame& frame) const;

private slots:
    void init();
    void cleanup();

    void defaultConstructor_Test();
    void connectToMakesTCPConnection_Test();
    void connectedSignalEmittedAfterConnectionMade_Test();
    void isConnectedTrueWhenInConnectedState_Test();
    void isConnectedFalseWhenNotInConnectedState_Test();
    void disconnectWillDisconnectASocketConnection_Test();
    void disconnectedSignalEmittedAfterADisconnection_Test();
    void sendframeSendsTheFrame_Test();
    void receivedReceivesAFrame_Test();
    void stateIsUnconnectedStateBeforeAnyConnectionMade_Test();
    void stateIsConnectingStateAfterToldToConnectButNotYetConnected_Test();
    void stateIsConnectedStateAfterConnectionHasBeenMade_Test();
    void stateIsUnconnectedStateAfterGivenDisconnect_Test();
    void autoReconnectDefaultsToFalse_Test();
    void autoReconnectTrueAfterSetAutoReconnectTrue_Test();
    void willNotAutoReconnectIfAutoReconnectIsSetFalse_Test();
    void willAutoReconnectIfAutoReconnectIsSetTrue_Test();
};

NetworkTests::NetworkTests()
    : _uut(NULL)
{
}

NetworkTests::~NetworkTests()
{
}

void NetworkTests::init()
{
    _uut.reset(new QMQTT::Network);
}

void NetworkTests::cleanup()
{
    _uut.reset();
}

QByteArray NetworkTests::serializeFrame(QMQTT::Frame& frame) const
{
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    QDataStream out(&buffer);
    frame.write(out);
    return bytes;
}

void NetworkTests::flushEvents()
{
    while (QCoreApplication::hasPendingEvents())
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
    }
}

void NetworkTests::defaultConstructor_Test()
{
    QCOMPARE(_uut->isConnectedToHost(), false);
    QCOMPARE(_uut->autoReconnect(), false);
    QCOMPARE(_uut->state(), QAbstractSocket::UnconnectedState);
}

void NetworkTests::connectToMakesTCPConnection_Test()
{
    TcpServer server;
    _uut->connectToHost(server.serverAddress().toString(), server.serverPort());
    bool timedOut = false;
    bool connectionMade = server.waitForNewConnection(5000, &timedOut);

    QCOMPARE(connectionMade, true);
    QCOMPARE(timedOut, false);
}

void NetworkTests::connectedSignalEmittedAfterConnectionMade_Test()
{
    TcpServer server;
    QSignalSpy spy(_uut.data(), &QMQTT::Network::connected);
    _uut->connectToHost(server.serverAddress().toString(), server.serverPort());
    QCOMPARE(spy.count(), 0);

    flushEvents();
    QCOMPARE(spy.count(), 1);
}

void NetworkTests::isConnectedTrueWhenInConnectedState_Test()
{
    TcpServer server;
    _uut->connectToHost(server.serverAddress().toString(), server.serverPort());
    flushEvents();

    QCOMPARE(_uut->isConnectedToHost(), true);
}

void NetworkTests::isConnectedFalseWhenNotInConnectedState_Test()
{
    QCOMPARE(_uut->isConnectedToHost(), false);
}

void NetworkTests::disconnectWillDisconnectASocketConnection_Test()
{
    TcpServer server;
    _uut->connectToHost(server.serverAddress().toString(), server.serverPort());
    flushEvents();
    QCOMPARE(_uut->isConnectedToHost(), true);

    _uut->disconnectFromHost();
    flushEvents();

    QCOMPARE(_uut->isConnectedToHost(), false);
}

void NetworkTests::disconnectedSignalEmittedAfterADisconnection_Test()
{
    TcpServer server;
    _uut->connectToHost(server.serverAddress().toString(), server.serverPort());
    flushEvents();
    QCOMPARE(_uut->isConnectedToHost(), true);

    QSignalSpy spy(_uut.data(), &QMQTT::Network::disconnected);
    QCOMPARE(spy.count(), 0);

    _uut->disconnectFromHost();
    flushEvents();

    QCOMPARE(spy.count(), 1);
}

void NetworkTests::sendframeSendsTheFrame_Test()
{
    TcpServer server;
    _uut->connectToHost(server.serverAddress().toString(), server.serverPort());
    flushEvents();
    QCOMPARE(_uut->isConnectedToHost(), true);

    QByteArray data("abc");
    QMQTT::Frame frame(42, data);
    _uut->sendFrame(frame);
    flushEvents();

    QCOMPARE(server.data(), serializeFrame(frame));
}

void NetworkTests::receivedReceivesAFrame_Test()
{
    TcpServer server;
    _uut->connectToHost(server.serverAddress().toString(), server.serverPort());
    flushEvents();
    QCOMPARE(_uut->isConnectedToHost(), true);

    qRegisterMetaType<QMQTT::Frame>("QMQTT::Frame");
    QSignalSpy spy(_uut.data(), &QMQTT::Network::received);

    QByteArray data("abc");
    QMQTT::Frame frameSent(42, data);
    QByteArray bytesSent = serializeFrame(frameSent);
    server.socket()->write(bytesSent);

    flushEvents();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).value<QMQTT::Frame>(), frameSent);
}

void NetworkTests::stateIsUnconnectedStateBeforeAnyConnectionMade_Test()
{
    QCOMPARE(_uut->state(), QAbstractSocket::UnconnectedState);
}

void NetworkTests::stateIsConnectingStateAfterToldToConnectButNotYetConnected_Test()
{
    TcpServer server;
    _uut->connectToHost(server.serverAddress().toString(), server.serverPort());
    QCOMPARE(_uut->state(), QAbstractSocket::ConnectingState);
}

void NetworkTests::stateIsConnectedStateAfterConnectionHasBeenMade_Test()
{
    TcpServer server;
    _uut->connectToHost(server.serverAddress().toString(), server.serverPort());
    flushEvents();
    QCOMPARE(_uut->state(), QAbstractSocket::ConnectedState);
}

void NetworkTests::stateIsUnconnectedStateAfterGivenDisconnect_Test()
{
    TcpServer server;
    _uut->connectToHost(server.serverAddress().toString(), server.serverPort());
    flushEvents();
    QCOMPARE(_uut->state(), QAbstractSocket::ConnectedState);

    _uut->disconnectFromHost();
    QCOMPARE(_uut->state(), QAbstractSocket::UnconnectedState);
}

void NetworkTests::autoReconnectDefaultsToFalse_Test()
{
    QCOMPARE(_uut->autoReconnect(), false);
}

void NetworkTests::autoReconnectTrueAfterSetAutoReconnectTrue_Test()
{
    _uut->setAutoReconnect(true);
    QCOMPARE(_uut->autoReconnect(), true);
}

void NetworkTests::willNotAutoReconnectIfAutoReconnectIsSetFalse_Test()
{
    TcpServer server;
    _uut->connectToHost(server.serverAddress().toString(), server.serverPort());
    flushEvents();
    QCOMPARE(_uut->isConnectedToHost(), true);

    server.socket()->disconnectFromHost();
    server.socket()->state() == QAbstractSocket::UnconnectedState
       || server.socket()->waitForDisconnected(5000);
    flushEvents();

    QCOMPARE(_uut->isConnectedToHost(), false);
}

void NetworkTests::willAutoReconnectIfAutoReconnectIsSetTrue_Test()
{
    QSKIP("Autoreconnect feature not yet enabled");

    _uut->setAutoReconnect(true);

    TcpServer server;
    _uut->connectToHost(server.serverAddress().toString(), server.serverPort());
    flushEvents();
    QCOMPARE(_uut->isConnectedToHost(), true);

    server.socket()->disconnectFromHost();
    server.socket()->waitForDisconnected(5000);
    flushEvents();

    QCOMPARE(_uut->autoReconnect(), true);
    QCOMPARE(_uut->isConnectedToHost(), true);
}

QTEST_MAIN(NetworkTests);
#include "networktests.moc"
