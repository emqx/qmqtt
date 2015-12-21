#include "clienttests.h"
#include "tcpserver.h"
#include <qmqtt_client.h>
#include <QTest>

ClientTests::ClientTests()
    : _uut(NULL)
{
}

ClientTests::~ClientTests()
{
}

void ClientTests::init()
{
    _uut.reset(new QMQTT::Client);
}

void ClientTests::cleanup()
{
    _uut.reset();
}

void ClientTests::flushEvents()
{
    while (QCoreApplication::hasPendingEvents())
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
    }
}

QSharedPointer<QTcpServer> ClientTests::createTcpServer()
{
    QSharedPointer<QTcpServer> server = QSharedPointer<QTcpServer>(new QTcpServer);
    server->listen(QHostAddress::LocalHost, static_cast<quint16>(8973));
    return server;
}

void ClientTests::connectClientToServer(QSharedPointer<QMQTT::Client> client,
                                        QSharedPointer<QTcpServer> server)
{
    client->setHost(server->serverAddress().toString());
    client->setPort(server->serverPort());
    client->connect();
}

void ClientTests::disconnectServerAndWaitForDisconnect(QSharedPointer<QTcpServer> server)
{
    QTcpSocket* socket = server->nextPendingConnection();
    socket->disconnectFromHost();
    socket->state() == QAbstractSocket::UnconnectedState
        || socket->waitForDisconnected(5000);
}

void ClientTests::constructorWithNoParameters_Test()
{
    // todo: eliminate annoying pass by reference in constructor
    QString host("localhost");
    QCOMPARE(_uut->host(), host);
    QCOMPARE(_uut->port(), static_cast<quint32>(1883));
    QVERIFY(NULL == _uut->parent());
}

void ClientTests::constructorWithHost_Test()
{
    _uut.reset(new QMQTT::Client("8.8.8.8"));

    QString host("8.8.8.8");
    QCOMPARE(_uut->host(), host);
    QCOMPARE(_uut->port(), static_cast<quint32>(1883));
    QVERIFY(NULL == _uut->parent());
}

void ClientTests::constructorWithHostAndPort_Test()
{
    _uut.reset(new QMQTT::Client("8.8.8.8", 8883));

    QString host("8.8.8.8");
    QCOMPARE(_uut->host(), host);
    QCOMPARE(_uut->port(), static_cast<quint32>(8883));
    QVERIFY(NULL == _uut->parent());
}

void ClientTests::constructorWithHostPortAndParent_Test()
{
    QObject parent;
    _uut.reset(new QMQTT::Client("8.8.8.8", 8883, &parent));

    QString host("8.8.8.8");
    QCOMPARE(_uut->host(), host);
    QCOMPARE(_uut->port(), static_cast<quint32>(8883));
    QCOMPARE(_uut->parent(), &parent);
    _uut.reset();
}

void ClientTests::hostReturnsHostValue_Test()
{
    QString host("localhost");
    QCOMPARE(_uut->host(), host);
}

void ClientTests::setHostSetsHostValue_Test()
{
    QString host("8.8.8.8");
    _uut->setHost(host);
    QCOMPARE(_uut->host(), host);
}

void ClientTests::portReturnsPortValue_Test()
{
    QCOMPARE(_uut->port(), static_cast<quint32>(1883));
}

void ClientTests::setPortSetsPortValue_Test()
{
    _uut->setPort(8883);
    QCOMPARE(_uut->port(), static_cast<quint32>(8883));
}

void ClientTests::clientIdReturnsClientId_Test()
{
    QCOMPARE(_uut->clientId(), QString());
}

void ClientTests::setClientIdSetsClientId_Test()
{
    _uut->setClientId("Client");
    QCOMPARE(_uut->clientId(), QString("Client"));
}

void ClientTests::usernameReturnsUsername_Test()
{
    QCOMPARE(_uut->username(), QString());
}

void ClientTests::setUsernameSetsUsername_Test()
{
    _uut->setUsername("Username");
    QCOMPARE(_uut->username(), QString("Username"));
}

void ClientTests::passwordReturnsPassword_Test()
{
    QCOMPARE(_uut->password(), QString());
}

void ClientTests::setPasswordSetsPassword_Test()
{
    _uut->setPassword("Password");
    QCOMPARE(_uut->password(), QString("Password"));
}

void ClientTests::keepaliveReturnsKeepAlive_Test()
{
    QCOMPARE(_uut->keepalive(), 300);
}

void ClientTests::setKeepAliveSetsKeepAlive_Test()
{
    _uut->setKeepAlive(400);
    QCOMPARE(_uut->keepalive(), 400);
}

void ClientTests::cleansessReturnsCleansess_Test()
{
    QCOMPARE(_uut->cleansess(), false);
}

void ClientTests::setCleansessSetsCleansess_Test()
{
    _uut->setCleansess(true);
    QCOMPARE(_uut->cleansess(), true);
}

void ClientTests::connectWillMakeTCPConnection_Test()
{
    QSharedPointer<QTcpServer> server = createTcpServer();
    connectClientToServer(_uut, server);
    bool connected = server->waitForNewConnection(5000);

    QCOMPARE(connected, true);
}

void ClientTests::isConnectedIsFalseWhenNotConnected_Test()
{
    QCOMPARE(_uut->isConnected(), false);
}

void ClientTests::isConnectedIsTrueWhenConnected_Test()
{
    QSharedPointer<QTcpServer> server = createTcpServer();
    connectClientToServer(_uut, server);
    server->waitForNewConnection(5000);
    flushEvents();

    QCOMPARE(_uut->isConnected(), true);
}

void ClientTests::autoReconnectDefaultsToFalse_Test()
{
    QCOMPARE(_uut->autoReconnect(), false);
}

void ClientTests::autoReconnectCanBeSetToTrue_Test()
{
    _uut->setAutoReconnect(true);
    QCOMPARE(_uut->autoReconnect(), true);
}

void ClientTests::autoReconnectDoesNotReconnect_Test()
{
    QSharedPointer<QTcpServer> server = createTcpServer();
    connectClientToServer(_uut, server);
    server->waitForNewConnection(5000);
    flushEvents();

    QCOMPARE(_uut->isConnected(), true);

    disconnectServerAndWaitForDisconnect(server);

    flushEvents();
    QCOMPARE(_uut->isConnected(), false);
}

void ClientTests::willDefaultsToNull_Test()
{
    QVERIFY(NULL == _uut->will());
}

void ClientTests::setWillSetsAWill_Test()
{
    _uut->setWill(new QMQTT::Will("topic", "message", 123, true));

    QVERIFY(NULL != _uut->will());
    QCOMPARE(_uut->will()->qos(), static_cast<quint8>(123));
    QCOMPARE(_uut->will()->retain(), true);
    QCOMPARE(_uut->will()->topic(), QString("topic"));
    QCOMPARE(_uut->will()->message(), QString("message"));
}

void ClientTests::stateDefaultsToStateInit_Test()
{
    QCOMPARE(_uut->state(), QMQTT::STATE_INIT);
}

// todo: unused state variable for now
void ClientTests::stateStuckOnStateInitEvenWhenConnectedToSocket_Test()
{
    QSharedPointer<QTcpServer> server = createTcpServer();
    connectClientToServer(_uut, server);
    server->waitForNewConnection(5000);
    flushEvents();

    QCOMPARE(_uut->state(), QMQTT::STATE_INIT);
}

// 1 << 1

const quint8 CONNECT_TYPE = 0x10;
const quint8 CONNACK_TYPE = 0x20;
const quint8 PUBLISH_TYPE = 0x30;
const quint8 PUBACK_TYPE = 0x40;
const quint8 PUBREC_TYPE = 0x50;
const quint8 PUBREL_TYPE = 0x60;
const quint8 PUBCOMP_TYPE = 0x70;
const quint8 SUBSCRIBE_TYPE = 0x80;
const quint8 SUBACK_TYPE = 0x90;
const quint8 UNSUBSCRIBE_TYPE = 0xA0;
const quint8 UNSUBACK_TYPE = 0xB0;
const quint8 PINGREQ_TYPE = 0xC0;
const quint8 PINGRESP_TYPE = 0xD0;
const quint8 DISCONNECT_TYPE = 0xE0;
const quint8 QOS0 = 0x00;
const quint8 QOS1 = 0x02;
const quint8 QOS2 = 0x04;

quint64 ClientTests::readEncodedLength(QDataStream& in)
{
    quint64 length = 0;
    quint8 byte = 0;
    int shift = 0;
    do {
        in >> byte;
        length |= (byte & 0x7f) << shift;
        shift += 7;
    } while (byte & 0x80);
    return length;
}

quint8 ClientTests::readByte(QDataStream& in)
{
    quint8 byte = 0;
    in >> byte;
    return byte;
}

quint16 ClientTests::readInt(QDataStream& in)
{
    quint16 i = 0;
    in >> i;
    return i;
}

QByteArray ClientTests::readByteArray(QDataStream &in, const quint64 length)
{
    QByteArray byteArray(length, ' ');
    int actualLength = in.readRawData(byteArray.data(), length);
    byteArray.resize(actualLength);
    return byteArray;
}

QString ClientTests::readString(QDataStream& in)
{
    quint16 length = readInt(in);
    return QString::fromUtf8(readByteArray(in, length));
}

void ClientTests::publishWillPublishAMessage()
{
    QSharedPointer<TcpServer> server = QSharedPointer<TcpServer>(new TcpServer);

    _uut->setHost(server->serverAddress().toString());
    _uut->setPort(server->serverPort());
    _uut->connect();

    server->waitForNewConnection(5000);
    flushEvents();

    QByteArray payload("payload");
    QMQTT::Message message(222, "/topic", payload);
    _uut->publish(message);
    flushEvents();

    QByteArray byteArray = server->data();
    QDataStream in(&byteArray, QBuffer::ReadOnly);

    quint8 header = readByte(in);
    quint64 numberOfBytes = readEncodedLength(in);
    readByteArray(in, numberOfBytes);
    QCOMPARE(header, static_cast<quint8>(CONNECT_TYPE | QOS1));

    header = readByte(in);
    numberOfBytes = readEncodedLength(in);
    readByteArray(in, numberOfBytes);
    QCOMPARE(header, static_cast<quint8>(PUBLISH_TYPE | QOS0));
}

/*
public slots:
    quint16 publish(Message &message);
    void puback(quint8 type, quint16 msgid);
//    void pubrec(int msgid);
//    void pubrel(int msgid);
//    void pubcomp(int msgid);
    quint16 subscribe(const QString &topic, quint8 qos);
    void unsubscribe(const QString &topic);
    void ping();
    void disconnect();

signals:
    void connected();
    void error(QAbstractSocket::SocketError);
    void connacked(quint8 ack);
    //send PUBLISH and receive PUBACK
    void published(QMQTT::Message &message);
    void pubacked(quint8 type, quint16 msgid);
    //receive PUBLISH
    void received(const QMQTT::Message &message);
    //send SUBSCRIBE and receive SUBACKED
    void subscribed(const QString &topic);
    void subacked(quint16 mid, quint8 qos);
    //send UNSUBSCRIBE and receive UNSUBACKED
    void unsubscribed(const QString &topic);
    void unsubacked(quint16 mid);
    //receive PINGRESP
    void pong();
    void disconnected();
*/
