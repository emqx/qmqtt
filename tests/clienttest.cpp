#include "networkmock.h"
#include <qmqtt_client.h>
#include <qmqtt_message.h>
#include <qmqtt_frame.h>
#include <qmqtt_connackpacket.h>
#include <qmqtt_publishpacket.h>
#include <QSharedPointer>
#include <QSignalSpy>
#include <QCoreApplication>
#include <QDataStream>
#include <gmock/gmock.h>

using namespace testing;

const QHostAddress HOST_ADDRESS = QHostAddress("8.8.8.8");
const quint16 PORT = 8883;

class ClientTest : public Test
{
public:
    explicit ClientTest()
        : _networkMock(new NetworkMock)
        , _client(new QMQTT::Client(_networkMock))
    {
        qRegisterMetaType<QMQTT::ClientError>("QMQTT::ClientError");
    }
    virtual ~ClientTest() {}

    NetworkMock* _networkMock;
    QSharedPointer<QMQTT::Client> _client;

    quint8 getHeaderType(const quint8 header)
    {
        return header & 0xF0;
    }
};

TEST_F(ClientTest, constructorWithNoParameters_Test)
{
    EXPECT_EQ(QHostAddress::LocalHost, _client->host());
    EXPECT_EQ(1883, _client->port());
    EXPECT_THAT(_client->parent(), IsNull());
}

TEST_F(ClientTest, constructorWithHost_Test)
{
    _client.reset(new QMQTT::Client(HOST_ADDRESS));

    EXPECT_EQ(HOST_ADDRESS, _client->host());
    EXPECT_EQ(1883, _client->port());
    EXPECT_THAT(_client->parent(), IsNull());
}

TEST_F(ClientTest, constructorWithHostAndPort_Test)
{
    _client.reset(new QMQTT::Client(HOST_ADDRESS, PORT));

    EXPECT_EQ(HOST_ADDRESS, _client->host());
    EXPECT_EQ(PORT, _client->port());
    EXPECT_THAT(_client->parent(), IsNull());
}

TEST_F(ClientTest, constructorWithHostPortAndParent_Test)
{
    QObject parent;
    _client.reset(new QMQTT::Client(HOST_ADDRESS, PORT, &parent));

    EXPECT_EQ(HOST_ADDRESS, _client->host());
    EXPECT_EQ(PORT, _client->port());
    EXPECT_EQ(&parent, _client->parent());
    _client.reset();
}

TEST_F(ClientTest, hostReturnsHostValue_Test)
{
    EXPECT_EQ(QHostAddress::LocalHost, _client->host());
}

TEST_F(ClientTest, setHostSetsHostValue_Test)
{
    _client->setHost(HOST_ADDRESS);
    EXPECT_EQ(HOST_ADDRESS, _client->host());
}

TEST_F(ClientTest, portReturnsPortValue_Test)
{
    EXPECT_EQ(1883, _client->port());
}

TEST_F(ClientTest, setPortSetsPortValue_Test)
{
    _client->setPort(PORT);
    EXPECT_EQ(PORT, _client->port());
}

TEST_F(ClientTest, clientIdReturnsClientId_Test)
{
    EXPECT_FALSE(_client->clientId().isEmpty());
}

TEST_F(ClientTest, setClientIdSetsClientId_Test)
{
    _client->setClientId("Client");
    EXPECT_EQ("Client", _client->clientId());
}

TEST_F(ClientTest, setClientIdCannotSetEmptyString_Test)
{
    _client->setClientId("");
    EXPECT_FALSE(_client->clientId().isEmpty());
}

TEST_F(ClientTest, usernameReturnsUsername_Test)
{
    EXPECT_EQ(QString(), _client->username());
}

TEST_F(ClientTest, setUsernameSetsUsername_Test)
{
    _client->setUsername("Username");
    EXPECT_EQ("Username", _client->username());
}

TEST_F(ClientTest, passwordReturnsPassword_Test)
{
    EXPECT_EQ(QString(), _client->password());
}

TEST_F(ClientTest, setPasswordSetsPassword_Test)
{
    _client->setPassword("Password");
    EXPECT_EQ("Password", _client->password());
}

TEST_F(ClientTest, keepAliveReturnsKeepAlive_Test)
{
    EXPECT_EQ(300, _client->keepAlive());
}

TEST_F(ClientTest, setKeepAliveSetsKeepAlive_Test)
{
    _client->setKeepAlive(400);
    EXPECT_EQ(400, _client->keepAlive());
}

TEST_F(ClientTest, CleanSessionReturnsCleanSession_Test)
{
    EXPECT_FALSE(_client->cleanSession());
}

TEST_F(ClientTest, setCleanSessionSetsCleanSession_Test)
{
    _client->setCleanSession(true);
    EXPECT_TRUE(_client->cleanSession());
}

TEST_F(ClientTest, connectToHostWillCallNetworkConnectToHost)
{
    EXPECT_CALL(*_networkMock, connectToHost(Eq(QHostAddress::LocalHost), Eq(1883)));
    _client->connectToHost();
}

TEST_F(ClientTest, isConnectedToHostIsFalseWhenNetworkIsNotConnected_Test)
{
    EXPECT_CALL(*_networkMock, isConnectedToHost()).WillOnce(Return(false));
    EXPECT_FALSE(_client->isConnectedToHost());
}

TEST_F(ClientTest, isConnectedToHostIsTrueWhenNetworkIsConnectedToHost_Test)
{
    EXPECT_CALL(*_networkMock, isConnectedToHost()).WillOnce(Return(true));
    EXPECT_TRUE(_client->isConnectedToHost());
}

TEST_F(ClientTest, autoReconnectIsTrueIfNetworkAutoReconnectIsTrue_Test)
{
    EXPECT_CALL(*_networkMock, autoReconnect()).WillOnce(Return(true));
    EXPECT_TRUE(_client->autoReconnect());
}

TEST_F(ClientTest, autoReconnectIsFalseIfNetworkAutoReconnectIsFalse_Test)
{
    EXPECT_CALL(*_networkMock, autoReconnect()).WillOnce(Return(false));
    EXPECT_FALSE(_client->autoReconnect());
}

TEST_F(ClientTest, setAutoReconnectSetsNetworkAutoReconnect_Test)
{
    EXPECT_CALL(*_networkMock, setAutoReconnect(true));
    _client->setAutoReconnect(true);
}

TEST_F(ClientTest, setAutoReconnectIntervalSetsNetworkAutoReconnectInterval_Test)
{
    EXPECT_CALL(*_networkMock, setAutoReconnectInterval(10000));
    _client->setAutoReconnectInterval(10000);
}

TEST_F(ClientTest, autoReconnectIntervalIsValueOfNetworkAutoReconnect_Test)
{
    EXPECT_CALL(*_networkMock, autoReconnectInterval()).WillOnce(Return(10000));
    EXPECT_TRUE(_client->autoReconnectInterval());
}

TEST_F(ClientTest, willTopicDefaultsToNull_Test)
{
    EXPECT_TRUE(_client->willTopic().isNull());
}

TEST_F(ClientTest, setWillTopicSetsAWillTopic_Test)
{
    _client->setWillTopic("topic");
    EXPECT_EQ("topic", _client->willTopic());
}

TEST_F(ClientTest, willQosDefaultsToZero_Test)
{
    EXPECT_EQ(0, _client->willQos());
}

TEST_F(ClientTest, setWillQosSetsAWillQos_Test)
{
    _client->setWillQos(2);
    EXPECT_EQ(2, _client->willQos());
}

TEST_F(ClientTest, willRetainDefaultsToFalse_Test)
{
    EXPECT_FALSE(_client->willRetain());
}

TEST_F(ClientTest, setWillRetainSetsAWillRetainToTrue_Test)
{
    _client->setWillRetain(true);
    EXPECT_TRUE(_client->willRetain());
}

TEST_F(ClientTest, willMessageDefaultsToNull_Test)
{
    EXPECT_TRUE(_client->willMessage().isNull());
}

TEST_F(ClientTest, setWillMessageSetsAWillMessageTest)
{
    _client->setWillMessage("message");
    EXPECT_EQ("message", _client->willMessage());
}

TEST_F(ClientTest, connectionStateReturnsStateInit_Test)
{
    EXPECT_EQ(QMQTT::STATE_INIT, _client->connectionState());
}

TEST_F(ClientTest, connectionStateReturnsStateInitEvenAfterConnected_Test)
{
    EXPECT_CALL(*_networkMock, sendFrame(_));

    emit _networkMock->connected();

    EXPECT_EQ(QMQTT::STATE_INIT, _client->connectionState());
}

TEST_F(ClientTest, connectSendsConnectMessage_Test)
{
    QMQTT::Frame frame;
    EXPECT_CALL(*_networkMock, sendFrame(_)).WillOnce(SaveArg<0>(&frame));

    emit _networkMock->connected();

    EXPECT_EQ(QMQTT::ConnectType, frame._header >> 4);
}

TEST_F(ClientTest, publishSendsPublishMessage_Test)
{
    QMQTT::Frame frame;
    EXPECT_CALL(*_networkMock, sendFrame(_)).WillOnce(SaveArg<0>(&frame));

    QMQTT::Message message(222, "topic", QByteArray("payload"));
    _client->publish(message);

    EXPECT_EQ(QMQTT::PublishType, frame._header >> 4);
}

TEST_F(ClientTest, subscribeSendsSubscribeMessage_Test)
{
    QMQTT::Frame frame;
    EXPECT_CALL(*_networkMock, sendFrame(_)).WillOnce(SaveArg<0>(&frame));

    _client->subscribe("topic", QMQTT::Qos2);

    EXPECT_EQ(QMQTT::SubscribeType, frame._header >> 4);
}

// todo: these are internal, test them as we can (all are puback types?)
//    void puback(quint8 type, quint16 msgid);
//    void pubrec(int msgid);
//    void pubrel(int msgid);
//    void pubcomp(int msgid);

// todo: what happens if not already subscribed?
TEST_F(ClientTest, unsubscribeSendsUnsubscribeMessage_Test)
{
    QMQTT::Frame frame;
    EXPECT_CALL(*_networkMock, sendFrame(_)).WillOnce(SaveArg<0>(&frame));

    _client->unsubscribe("topic");

    EXPECT_EQ(QMQTT::UnsubscribeType, frame._header >> 4);
    // todo: test the topic
}

TEST_F(ClientTest, disconnectSendsDisconnectMessageAndNetworkDisconnect_Test)
{
    QMQTT::Frame frame;
    EXPECT_CALL(*_networkMock, sendFrame(_)).WillOnce(SaveArg<0>(&frame));
    EXPECT_CALL(*_networkMock, disconnectFromHost());

    _client->disconnectFromHost();

    EXPECT_EQ(QMQTT::DisconnectType, frame._header >> 4);
}

// todo: verify pingreq sent from client, will require timer interface and mock

// todo: this shouldn't emit connected until connect packet received
TEST_F(ClientTest, networkConnectEmitsConnectedSignal_Test)
{
    EXPECT_CALL(*_networkMock, sendFrame(_));
    QSignalSpy spy(_client.data(), &QMQTT::Client::connected);

    emit _networkMock->connected();

    EXPECT_EQ(1, spy.count());
}

TEST_F(ClientTest, networkReceivedSendsConnackDoesNotEmitConnectedSignal_Test)
{
    QSignalSpy spy(_client.data(), &QMQTT::Client::connected);

    QMQTT::ConnackPacket packet;
    emit _networkMock->received(packet.toFrame());

    EXPECT_EQ(0, spy.count());
}

// todo: receive connack_type should start keepalive
// todo: receive disconnect_type message should stop keepalive
//  we need a timer interface to test timer start and stop correctly

// todo: connack, connection accepted
// todo: connack, connection refused, unnacceptable protocol
// todo: connack, connection refused, identifier rejected
TEST_F(ClientTest, publishEmitsPublishedSignal_Test)
{
    EXPECT_CALL(*_networkMock, sendFrame(_));
    qRegisterMetaType<QMQTT::Message>("QMQTT::Message&");
    QSignalSpy spy(_client.data(), &QMQTT::Client::published);
    QMQTT::Message message(222, "topic", QByteArray("payload"));

    _client->publish(message);

    EXPECT_EQ(1, spy.count());
    EXPECT_EQ(message, spy.at(0).at(0).value<QMQTT::Message>());
}

// todo: network received sends a puback, test what happens
// todo: client sends puback, test what happens

// todo: two different response types for different QoS levels
TEST_F(ClientTest, networkReceivedSendsPublishEmitsReceivedSignal_Test)
{
    QSignalSpy spy(_client.data(), &QMQTT::Client::received);

    QMQTT::Frame frame = QMQTT::PublishPacket().toFrame();
    emit _networkMock->received(frame);

    EXPECT_EQ(1, spy.count());
}

// todo: should happen on suback
TEST_F(ClientTest, subscribeEmitsSubscribedSignal_Test)
{
    EXPECT_CALL(*_networkMock, sendFrame(_));
    QSignalSpy spy(_client.data(), &QMQTT::Client::subscribed);

    _client->subscribe("topic", QMQTT::Qos2);

    EXPECT_EQ(1, spy.count());
    EXPECT_EQ("topic", spy.at(0).at(0).toString());
}

// todo: network received sends suback triggers a subscribed signal (other things?)

// todo: should happen on unsuback
TEST_F(ClientTest, unsubscribeEmitsUnsubscribedSignal_Test)
{
    EXPECT_CALL(*_networkMock, sendFrame(_));
    QSignalSpy spy(_client.data(), &QMQTT::Client::unsubscribed);

    _client->unsubscribe("topic");

    EXPECT_EQ(1, spy.count());
    EXPECT_EQ("topic", spy.at(0).at(0).toString());
}

// todo: network received sends unsuback then emit unsubscribed signal (only then?)

TEST_F(ClientTest, networkDisconnectedEmitsDisconnectedSignal_Test)
{
    QSignalSpy spy(_client.data(), &QMQTT::Client::disconnected);

    emit _networkMock->disconnected();

    EXPECT_EQ(1, spy.count());
}

TEST_F(ClientTest, clientEmitsErrorWhenNetworkEmitsError_Test)
{
    QSignalSpy spy(_client.data(), &QMQTT::Client::error);
    emit _networkMock->error(QAbstractSocket::ConnectionRefusedError);
    EXPECT_EQ(1, spy.count());
    EXPECT_EQ(QMQTT::SocketConnectionRefusedError,
              spy.at(0).at(0).value<QMQTT::ClientError>());
}
