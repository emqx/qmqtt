#include "qmqtt_client.h"
#include "qmqtt_will.h"
#include "qmqtt_message.h"
#include "qmqtt_frame.h"
#include "tcpserver.h"
#include <QTest>
#include <QObject>
#include <QSharedPointer>
#include <QTcpSocket>
#include <QSignalSpy>

class ClientTests : public QObject
{
    Q_OBJECT
public:
    explicit ClientTests();
    virtual ~ClientTests();

    QSharedPointer<QMQTT::Client> _uut;

    void flushEvents();
    QSharedPointer<QTcpServer> createTcpServer();
    void connectClientToServer(QSharedPointer<QMQTT::Client> client,
                               QSharedPointer<QTcpServer> server);
    void disconnectServerAndWaitForDisconnect(QSharedPointer<QTcpServer> server);
    quint64 readEncodedLength(QDataStream& in);
    quint8 readByte(QDataStream& in);
    void writeByte(QDataStream& out, quint8 byte);
    quint16 readInt(QDataStream& in);
    void writeInt(QDataStream& out, quint16 i);
    QByteArray readByteArray(QDataStream& in, const quint64 length);
    void writeByteArray(QDataStream& out, const QByteArray byteArray);
    QString readString(QDataStream& in);
    void writeString(QDataStream& out, const QString string);

private slots:
    void init();
    void cleanup();

    void constructorWithNoParameters_Test();
    void constructorWithHost_Test();
    void constructorWithHostAndPort_Test();
    void constructorWithHostPortAndParent_Test();
    // setters and getter tests
    void hostReturnsHostValue_Test();
    void portReturnsPortValue_Test();
    void setHostSetsHostValue_Test();
    void setPortSetsPortValue_Test();
    void clientIdReturnsClientId_Test();
    void setClientIdSetsClientId_Test();
    void usernameReturnsUsername_Test();
    void setUsernameSetsUsername_Test();
    void passwordReturnsPassword_Test();
    void setPasswordSetsPassword_Test();
    void keepaliveReturnsKeepAlive_Test();
    void setKeepAliveSetsKeepAlive_Test();
    void cleanSessionReturnsCleanSession_Test();
    void setCleanSessionSetsCleanSession_Test();
    void connectWillMakeTCPConnection_Test();
    void isConnectedIsFalseWhenNotConnected_Test();
    void isConnectedIsTrueWhenConnected_Test();
    void autoReconnectDefaultsToFalse_Test();
    void autoReconnectCanBeSetToTrue_Test();
    void autoReconnectDoesNotReconnect_Test();
    void willDefaultsToNull_Test();
    void setWillSetsAWill_Test();
    void stateDefaultsToStateInit_Test();
    void stateRemainsInStateInitWhenConnected_Test();
    // message send tests
    void connectSendsConnectMessage_Test();
    void publishSendsPublishMessage_Test();
    void subscribeSendsSubscribeMessage_Test();
    void pubackSendsPubackMessage_Test();
    void unsubscribeSendsUnsubscribeMessage_Test();
    void disconnectSendsDisconnectMessage_Test();
    void pingSendsPingMessage_Test();
    // signals tests
    void connectEmitsConnectedSignal_Test();
    void tcpSocketErrorEmitsErrorSignal_Test();
    void receivingConnackEmitsConnackedSignal_Test();
    void publishEmitsPublishedSignal_Test();
    void receivingPubackEmitsPubackedSignal_Test();
    void receivingPublishEmitsReceivedSignal_Test();
    void subscribeEmitsSubscribedSignal_Test();
    void receivingSubackEmitsSubackedSignal_Test();
    void unsubscribeEmitsUnsubscribedSignal_Test();
    void receivingUnsubackEmitsUnsubackedSignal_Test();
    void receivingPingrespEmitsPongSignal_Test();
    void tcpSocketDisconnectEmitsDisconnectedSignal_Test();
};

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
    client->connectToHost();
}

void ClientTests::disconnectServerAndWaitForDisconnect(QSharedPointer<QTcpServer> server)
{
    QTcpSocket* socket = server->nextPendingConnection();
    socket->disconnectFromHost();
    socket->state() == QAbstractSocket::UnconnectedState
        || socket->waitForDisconnected(5000);
}

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

void ClientTests::writeByte(QDataStream& out, quint8 byte)
{
    out << byte;
}

quint16 ClientTests::readInt(QDataStream& in)
{
    quint16 i = 0;
    in >> i;
    return i;
}

void ClientTests::writeInt(QDataStream& out, const quint16 i)
{
    out << i;
}

QByteArray ClientTests::readByteArray(QDataStream& in, const quint64 length)
{
    QByteArray byteArray(length, ' ');
    int actualLength = in.readRawData(byteArray.data(), length);
    byteArray.resize(actualLength);
    return byteArray;
}

void ClientTests::writeByteArray(QDataStream& out, const QByteArray byteArray)
{
    out.writeRawData(byteArray.constData(), byteArray.size());
}

QString ClientTests::readString(QDataStream& in)
{
    quint16 length = readInt(in);
    return QString::fromUtf8(readByteArray(in, length));
}

void ClientTests::writeString(QDataStream& out, const QString string)
{
    QByteArray byteArray = string.toUtf8();
    writeInt(out, byteArray.size());
    writeByteArray(out, byteArray);
}

void ClientTests::constructorWithNoParameters_Test()
{
    // todo: eliminate annoying pass by reference in constructor
    QString host("localhost");
    QCOMPARE(_uut->host(), host);
    QCOMPARE(_uut->port(), static_cast<quint16>(1883));
    QVERIFY(NULL == _uut->parent());
}

void ClientTests::constructorWithHost_Test()
{
    _uut.reset(new QMQTT::Client("8.8.8.8"));

    QString host("8.8.8.8");
    QCOMPARE(_uut->host(), host);
    QCOMPARE(_uut->port(), static_cast<quint16>(1883));
    QVERIFY(NULL == _uut->parent());
}

void ClientTests::constructorWithHostAndPort_Test()
{
    _uut.reset(new QMQTT::Client("8.8.8.8", 8883));

    QString host("8.8.8.8");
    QCOMPARE(_uut->host(), host);
    QCOMPARE(_uut->port(), static_cast<quint16>(8883));
    QVERIFY(NULL == _uut->parent());
}

void ClientTests::constructorWithHostPortAndParent_Test()
{
    QObject parent;
    _uut.reset(new QMQTT::Client("8.8.8.8", 8883, &parent));

    QString host("8.8.8.8");
    QCOMPARE(_uut->host(), host);
    QCOMPARE(_uut->port(), static_cast<quint16>(8883));
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
    QCOMPARE(_uut->port(), static_cast<quint16>(1883));
}

void ClientTests::setPortSetsPortValue_Test()
{
    _uut->setPort(8883);
    QCOMPARE(_uut->port(), static_cast<quint16>(8883));
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

void ClientTests::cleanSessionReturnsCleanSession_Test()
{
    QCOMPARE(_uut->cleanSession(), false);
}

void ClientTests::setCleanSessionSetsCleanSession_Test()
{
    _uut->setCleanSession(true);
    QCOMPARE(_uut->cleanSession(), true);
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
    QCOMPARE(_uut->isConnectedToHost(), false);
}

void ClientTests::isConnectedIsTrueWhenConnected_Test()
{
    QSharedPointer<QTcpServer> server = createTcpServer();
    connectClientToServer(_uut, server);
    server->waitForNewConnection(5000);
    flushEvents();

    QCOMPARE(_uut->isConnectedToHost(), true);
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

    QCOMPARE(_uut->isConnectedToHost(), true);

    disconnectServerAndWaitForDisconnect(server);

    flushEvents();
    QCOMPARE(_uut->isConnectedToHost(), false);
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
void ClientTests::stateRemainsInStateInitWhenConnected_Test()
{
    QSharedPointer<QTcpServer> server = createTcpServer();
    connectClientToServer(_uut, server);
    server->waitForNewConnection(5000);
    flushEvents();

    QCOMPARE(_uut->state(), QMQTT::STATE_INIT);
}

void ClientTests::connectSendsConnectMessage_Test()
{
    QSharedPointer<TcpServer> server = QSharedPointer<TcpServer>(new TcpServer);

    connectClientToServer(_uut, server);

    server->waitForNewConnection(5000);
    flushEvents();

    QByteArray byteArray = server->data();
    QDataStream in(&byteArray, QIODevice::ReadOnly);

    quint8 header = readByte(in);
    quint64 numberOfBytes = readEncodedLength(in);
    readByteArray(in, numberOfBytes);
    QCOMPARE(header, static_cast<quint8>(CONNECT_TYPE | QOS1));
}

void ClientTests::publishSendsPublishMessage_Test()
{
    QSharedPointer<TcpServer> server = QSharedPointer<TcpServer>(new TcpServer);

    connectClientToServer(_uut, server);

    server->waitForNewConnection(5000);
    flushEvents();

    QByteArray payload("payload");
    QMQTT::Message message(222, "topic", payload);
    _uut->publish(message);
    flushEvents();

    QByteArray byteArray = server->data();
    QDataStream in(&byteArray, QIODevice::ReadOnly);

    quint8 header = readByte(in);
    quint64 numberOfBytes = readEncodedLength(in);
    readByteArray(in, numberOfBytes);
    QCOMPARE(header, static_cast<quint8>(CONNECT_TYPE | QOS1));

    header = readByte(in);
    numberOfBytes = readEncodedLength(in);
    readByteArray(in, numberOfBytes);
    QCOMPARE(header, static_cast<quint8>(PUBLISH_TYPE | QOS0));
}

void ClientTests::subscribeSendsSubscribeMessage_Test()
{
    QSharedPointer<TcpServer> server = QSharedPointer<TcpServer>(new TcpServer);

    connectClientToServer(_uut, server);

    server->waitForNewConnection(5000);
    flushEvents();

    _uut->subscribe("topic", QOS2);
    flushEvents();

    QByteArray byteArray = server->data();
    QDataStream in(&byteArray, QIODevice::ReadOnly);

    quint8 header = readByte(in);
    quint64 numberOfBytes = readEncodedLength(in);
    readByteArray(in, numberOfBytes);
    QCOMPARE(header, static_cast<quint8>(CONNECT_TYPE | QOS1));

    header = readByte(in);
    numberOfBytes = readEncodedLength(in);
    readByteArray(in, numberOfBytes);
    QCOMPARE(header, static_cast<quint8>(SUBSCRIBE_TYPE | QOS1));
}

// all these under puback
//    void puback(quint8 type, quint16 msgid);
//    void pubrec(int msgid);
//    void pubrel(int msgid);
//    void pubcomp(int msgid);
void ClientTests::pubackSendsPubackMessage_Test()
{
    QSharedPointer<TcpServer> server = QSharedPointer<TcpServer>(new TcpServer);

    connectClientToServer(_uut, server);

    server->waitForNewConnection(5000);
    flushEvents();

    _uut->puback(PUBACK, 42);
    flushEvents();

    QByteArray byteArray = server->data();
    QDataStream in(&byteArray, QIODevice::ReadOnly);

    quint8 header = readByte(in);
    quint64 numberOfBytes = readEncodedLength(in);
    readByteArray(in, numberOfBytes);
    QCOMPARE(header, static_cast<quint8>(CONNECT_TYPE | QOS1));

    header = readByte(in);
    numberOfBytes = readEncodedLength(in);
    readByteArray(in, numberOfBytes);
    QCOMPARE(header, static_cast<quint8>(PUBACK_TYPE | QOS0));
}

void ClientTests::unsubscribeSendsUnsubscribeMessage_Test()
{
    QSharedPointer<TcpServer> server = QSharedPointer<TcpServer>(new TcpServer);

    connectClientToServer(_uut, server);

    server->waitForNewConnection(5000);
    flushEvents();

    _uut->unsubscribe("topic");
    flushEvents();

    QByteArray byteArray = server->data();
    QDataStream in(&byteArray, QIODevice::ReadOnly);

    quint8 header = readByte(in);
    quint64 numberOfBytes = readEncodedLength(in);
    readByteArray(in, numberOfBytes);
    QCOMPARE(header, static_cast<quint8>(CONNECT_TYPE | QOS1));

    header = readByte(in);
    numberOfBytes = readEncodedLength(in);
    readByteArray(in, numberOfBytes);
    QCOMPARE(header, static_cast<quint8>(UNSUBSCRIBE_TYPE | QOS1));
}

// todo: what happens if not already subscribed?

void ClientTests::disconnectSendsDisconnectMessage_Test()
{
    QSharedPointer<TcpServer> server = QSharedPointer<TcpServer>(new TcpServer);

    connectClientToServer(_uut, server);

    server->waitForNewConnection(5000);
    flushEvents();

    _uut->disconnectFromHost();
    flushEvents();

    QByteArray byteArray = server->data();
    QDataStream in(&byteArray, QIODevice::ReadOnly);

    quint8 header = readByte(in);
    quint64 numberOfBytes = readEncodedLength(in);
    readByteArray(in, numberOfBytes);
    QCOMPARE(header, static_cast<quint8>(CONNECT_TYPE | QOS1));

    header = readByte(in);
    numberOfBytes = readEncodedLength(in);
    readByteArray(in, numberOfBytes);
    QCOMPARE(header, static_cast<quint8>(DISCONNECT_TYPE | QOS0));
}

void ClientTests::pingSendsPingMessage_Test()
{
    QSharedPointer<TcpServer> server = QSharedPointer<TcpServer>(new TcpServer);

    connectClientToServer(_uut, server);

    server->waitForNewConnection(5000);
    flushEvents();

    _uut->ping();
    flushEvents();

    QByteArray byteArray = server->data();
    QDataStream in(&byteArray, QIODevice::ReadOnly);

    quint8 header = readByte(in);
    quint64 numberOfBytes = readEncodedLength(in);
    readByteArray(in, numberOfBytes);
    QCOMPARE(header, static_cast<quint8>(CONNECT_TYPE | QOS1));

    header = readByte(in);
    numberOfBytes = readEncodedLength(in);
    readByteArray(in, numberOfBytes);
    QCOMPARE(header, static_cast<quint8>(PINGREQ_TYPE | QOS0));
}

void ClientTests::connectEmitsConnectedSignal_Test()
{
    QSharedPointer<TcpServer> server = QSharedPointer<TcpServer>(new TcpServer);

    QSignalSpy spy(_uut.data(), &QMQTT::Client::connected);

    connectClientToServer(_uut, server);

    server->waitForNewConnection(5000);
    flushEvents();

    QCOMPARE(spy.count(), 1);
}

void ClientTests::tcpSocketErrorEmitsErrorSignal_Test()
{
    QSignalSpy spy(_uut.data(), &QMQTT::Client::error);

    _uut->setHost(QHostAddress(TcpServer::HOST).toString());
    _uut->setPort(TcpServer::PORT);
    _uut->connectToHost();

    flushEvents();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).value<QAbstractSocket::SocketError>(), QAbstractSocket::ConnectionRefusedError);
}

// todo: connect starts keepalive

void ClientTests::receivingConnackEmitsConnackedSignal_Test()
{
    QSharedPointer<TcpServer> server = QSharedPointer<TcpServer>(new TcpServer);

    QSignalSpy spy(_uut.data(), &QMQTT::Client::connacked);

    connectClientToServer(_uut, server);

    server->waitForNewConnection(5000);
    flushEvents();

    QMQTT::Frame frame(CONNACK_TYPE, QByteArray(2, 0x0));
    QDataStream out(server->socket());
    frame.write(out);
    server->socket()->waitForBytesWritten(5000);
    flushEvents();

    QCOMPARE(spy.count(), 1);
    // todo: compare data received?
}

// todo: connack, connection accepted
// todo: connack, connection refused, unnacceptable protocol
// todo: connack, connection refused, identifier rejected

void ClientTests::publishEmitsPublishedSignal_Test()
{
    QSharedPointer<TcpServer> server = QSharedPointer<TcpServer>(new TcpServer);

    connectClientToServer(_uut, server);

    server->waitForNewConnection(5000);
    flushEvents();

    qRegisterMetaType<QMQTT::Message>("QMQTT::Message&");
    QSignalSpy spy(_uut.data(), &QMQTT::Client::published);

    QByteArray payload("payload");
    QMQTT::Message message(222, "topic", payload);
    _uut->publish(message);
    flushEvents();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).value<QMQTT::Message>(), message);
}

void ClientTests::receivingPubackEmitsPubackedSignal_Test()
{
    QSharedPointer<TcpServer> server = QSharedPointer<TcpServer>(new TcpServer);

    connectClientToServer(_uut, server);

    server->waitForNewConnection(5000);
    flushEvents();

    qRegisterMetaType<QMQTT::Message>("QMQTT::Message");
    QSignalSpy spy(_uut.data(), &QMQTT::Client::pubacked);

    QMQTT::Frame frame(PUBACK_TYPE, QByteArray(2, 0x0));
    QDataStream out(server->socket());
    frame.write(out);
    server->socket()->waitForBytesWritten(5000);
    flushEvents();

    QCOMPARE(spy.count(), 1);
    // todo: compare data received?
}

void ClientTests::receivingPublishEmitsReceivedSignal_Test()
{
    QSharedPointer<TcpServer> server = QSharedPointer<TcpServer>(new TcpServer);

    connectClientToServer(_uut, server);

    server->waitForNewConnection(5000);
    flushEvents();

    QSignalSpy spy(_uut.data(), &QMQTT::Client::received);

    QByteArray variableHeader;
    QDataStream variableHeaderDataStream(&variableHeader, QIODevice::WriteOnly);
    writeString(variableHeaderDataStream, "topic");
    QMQTT::Frame frame(PUBLISH_TYPE, variableHeader);
    QDataStream out(server->socket());
    frame.write(out);

    server->socket()->waitForBytesWritten(5000);
    flushEvents();

    QCOMPARE(spy.count(), 1);
    // todo: compare data received?
}

void ClientTests::subscribeEmitsSubscribedSignal_Test()
{
    QSharedPointer<TcpServer> server = QSharedPointer<TcpServer>(new TcpServer);

    connectClientToServer(_uut, server);

    server->waitForNewConnection(5000);
    flushEvents();

    QSignalSpy spy(_uut.data(), &QMQTT::Client::subscribed);

    _uut->subscribe("topic", QOS2);
    flushEvents();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toString(), QString("topic"));
}

void ClientTests::receivingSubackEmitsSubackedSignal_Test()
{
    QSharedPointer<TcpServer> server = QSharedPointer<TcpServer>(new TcpServer);

    connectClientToServer(_uut, server);

    server->waitForNewConnection(5000);
    flushEvents();

    QSignalSpy spy(_uut.data(), &QMQTT::Client::subacked);

    QByteArray variableHeader;
    QDataStream variableHeaderDataStream(&variableHeader, QIODevice::WriteOnly);
    writeInt(variableHeaderDataStream, 0);
    writeByte(variableHeaderDataStream, 0x00);
    QMQTT::Frame frame(SUBACK_TYPE, variableHeader);
    QDataStream out(server->socket());
    frame.write(out);

    server->socket()->waitForBytesWritten(5000);
    flushEvents();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).value<quint8>(), QOS0);
}

void ClientTests::unsubscribeEmitsUnsubscribedSignal_Test()
{
    QSharedPointer<TcpServer> server = QSharedPointer<TcpServer>(new TcpServer);

    connectClientToServer(_uut, server);

    server->waitForNewConnection(5000);
    flushEvents();

    QSignalSpy spy(_uut.data(), &QMQTT::Client::unsubscribed);

    _uut->unsubscribe("topic");
    flushEvents();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toString(), QString("topic"));
}

void ClientTests::receivingUnsubackEmitsUnsubackedSignal_Test()
{
    QSharedPointer<TcpServer> server = QSharedPointer<TcpServer>(new TcpServer);

    connectClientToServer(_uut, server);

    server->waitForNewConnection(5000);
    flushEvents();

    QSignalSpy spy(_uut.data(), &QMQTT::Client::unsubacked);

    QByteArray variableHeader;
    QDataStream variableHeaderDataStream(&variableHeader, QIODevice::WriteOnly);
    writeInt(variableHeaderDataStream, 13);
    QMQTT::Frame frame(UNSUBACK_TYPE, variableHeader);
    QDataStream out(server->socket());
    frame.write(out);

    server->socket()->waitForBytesWritten(5000);
    flushEvents();

    QCOMPARE(spy.count(), 1);
    // todo: msg id is always zero, it appears
    QCOMPARE(spy.at(0).at(0).value<quint16>(), static_cast<quint16>(0));
}

void ClientTests::receivingPingrespEmitsPongSignal_Test()
{
    QSharedPointer<TcpServer> server = QSharedPointer<TcpServer>(new TcpServer);

    connectClientToServer(_uut, server);

    server->waitForNewConnection(5000);
    flushEvents();

    QSignalSpy spy(_uut.data(), &QMQTT::Client::pong);

    QByteArray variableHeader;
    QMQTT::Frame frame(PINGRESP_TYPE, variableHeader);
    QDataStream out(server->socket());
    frame.write(out);

    server->socket()->waitForBytesWritten(5000);
    flushEvents();

    QCOMPARE(spy.count(), 1);
}

void ClientTests::tcpSocketDisconnectEmitsDisconnectedSignal_Test()
{
    QSharedPointer<TcpServer> server = QSharedPointer<TcpServer>(new TcpServer);

    connectClientToServer(_uut, server);

    server->waitForNewConnection(5000);
    flushEvents();

    QSignalSpy spy(_uut.data(), &QMQTT::Client::disconnected);

    server->socket()->disconnectFromHost();
    server->socket()->state() == QAbstractSocket::UnconnectedState
       || server->socket()->waitForDisconnected(5000);
    flushEvents();

    QCOMPARE(spy.count(), 1);
}

QTEST_MAIN(ClientTests);
#include "clienttests.moc"
