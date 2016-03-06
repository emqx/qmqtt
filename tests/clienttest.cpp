#include "networkmock.h"
#include "timermock.h"
#include <qmqtt_client.h>
#include <qmqtt_message.h>
#include <qmqtt_frame.h>
#include <qmqtt_connackpacket.h>
#include <qmqtt_connectpacket.h>
#include <qmqtt_subackpacket.h>
#include <qmqtt_publishpacket.h>
#include <qmqtt_unsubscribepacket.h>
#include <qmqtt_unsubackpacket.h>
#include <qmqtt_disconnectpacket.h>
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
        , _timerMock(new TimerMock)
        , _keepAliveTimerMock(new TimerMock)
        , _client(new QMQTT::Client(_networkMock, _timerMock, _keepAliveTimerMock))
    {
        qRegisterMetaType<QMQTT::ClientError>("QMQTT::ClientError");
    }
    virtual ~ClientTest() {}

    NetworkMock* _networkMock;
    TimerMock* _timerMock;
    TimerMock* _keepAliveTimerMock;
    QSharedPointer<QMQTT::Client> _client;
};

//------------------------------------------
// CONSTRUCTOR, SETTERS, and GETTERS
//------------------------------------------

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

TEST_F(ClientTest, connectToHostWillCallNetworkConnectToHost_Test)
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

//------------------------------------------
// CONNECTION STATE
//------------------------------------------

TEST_F(ClientTest, connectionStateReturnsInitializedState_Test)
{
    EXPECT_EQ(QMQTT::InitializedState, _client->connectionState());
}

TEST_F(ClientTest, connectionStateReturnsConnectingStateAfterConnectToHostCalled_Test)
{
    _client->connectToHost();

    EXPECT_EQ(QMQTT::ConnectingState, _client->connectionState());
}

TEST_F(ClientTest, connectionStateReturnsConnectedStateAfterConnackPacketReceived_Test)
{
    emit _networkMock->received(QMQTT::ConnackPacket().toFrame());

    EXPECT_EQ(QMQTT::ConnectedState, _client->connectionState());
}

TEST_F(ClientTest, connectionStateReturnsDisconnectedStateAfterDisconnectPacketReceived_Test)
{
    emit _networkMock->received(QMQTT::DisconnectPacket().toFrame());

    EXPECT_EQ(QMQTT::DisconnectedState, _client->connectionState());
}

TEST_F(ClientTest, connectionStateReturnsDisconnectedStateAfterNetworkDisconnects_Test)
{
    emit _networkMock->disconnected();

    EXPECT_EQ(QMQTT::DisconnectedState, _client->connectionState());
}

//------------------------------------------

TEST_F(ClientTest, networkConnecteWillSendConnectPacket_Test)
{
    QMQTT::Frame frame;
    ASSERT_NE(QMQTT::ConnectType, frame._header >> 4);
    EXPECT_CALL(*_networkMock, sendFrame(_)).WillOnce(SaveArg<0>(&frame));

    emit _networkMock->connected();

    EXPECT_EQ(QMQTT::ConnectType, frame._header >> 4);
}

TEST_F(ClientTest, publishWillSendPublishPacket_Test)
{
    QMQTT::Frame frame;
    ASSERT_NE(QMQTT::PublishType, frame._header >> 4);
    EXPECT_CALL(*_networkMock, sendFrame(_)).WillOnce(SaveArg<0>(&frame));

    QMQTT::Message message(222, "topic", QByteArray("payload"));
    _client->publish(message);

    EXPECT_EQ(QMQTT::PublishType, frame._header >> 4);
}

TEST_F(ClientTest, subscribeWillSendSubscribePacket_Test)
{
    QMQTT::Frame frame;
    ASSERT_NE(QMQTT::SubscribeType, frame._header >> 4);
    EXPECT_CALL(*_networkMock, sendFrame(_)).WillOnce(SaveArg<0>(&frame));

    _client->subscribe("topic", QMQTT::Qos2);

    EXPECT_EQ(QMQTT::SubscribeType, frame._header >> 4);
}

// todo: what happens if not already subscribed?
// will require session support
TEST_F(ClientTest, unsubscribeWillSendUnsubscribePacket_Test)
{
    QMQTT::Frame frame;
    ASSERT_NE(QMQTT::UnsubscribeType, frame._header >> 4);
    EXPECT_CALL(*_networkMock, sendFrame(_)).WillOnce(SaveArg<0>(&frame));

    _client->unsubscribe("topic");

    ASSERT_EQ(QMQTT::UnsubscribeType, frame._header >> 4);
    QMQTT::UnsubscribePacket unsubscribePacket = QMQTT::UnsubscribePacket::fromFrame(frame);
    ASSERT_EQ(1, unsubscribePacket.topicFilterList().size());
    EXPECT_EQ("topic", unsubscribePacket.topicFilterList().first());
}

TEST_F(ClientTest, disconnectFromHostSendsDisconnectPacket_Test)
{
    QMQTT::Frame frame;
    ASSERT_NE(QMQTT::DisconnectType, frame._header >> 4);
    EXPECT_CALL(*_networkMock, sendFrame(_)).WillOnce(SaveArg<0>(&frame));

    _client->disconnectFromHost();

    EXPECT_EQ(QMQTT::DisconnectType, frame._header >> 4);
}

TEST_F(ClientTest, disconnectFromHostCallsNetworkDisconnectFromHost_Test)
{
    EXPECT_CALL(*_networkMock, disconnectFromHost());

    _client->disconnectFromHost();
}

TEST_F(ClientTest, receivingConnackPacketEmitsConnectedSignal_Test)
{
    QSignalSpy spy(_client.data(), &QMQTT::Client::connected);

    QMQTT::ConnackPacket connackPacket;
    emit _networkMock->received(connackPacket.toFrame());

    EXPECT_EQ(1, spy.count());
}

TEST_F(ClientTest, receivingConnackPacketStartsKeepAliveTimer_Test)
{
    EXPECT_CALL(*_keepAliveTimerMock, start()).WillOnce(DoAll(
        Invoke(_timerMock, &QMQTT::TimerInterface::timeout),
        Return()));

    QMQTT::ConnackPacket connackPacket;
    emit _networkMock->received(connackPacket.toFrame());
}

TEST_F(ClientTest, receivingDisconnectPacketStopsKeepAliveTimer_Test)
{
    EXPECT_CALL(*_keepAliveTimerMock, stop());

    QMQTT::DisconnectPacket disconnectPacket;
    emit _networkMock->received(disconnectPacket.toFrame());
}

TEST_F(ClientTest, networkDisconnectedStopsKeepAliveTimer_Test)
{
    EXPECT_CALL(*_keepAliveTimerMock, stop());

    emit _networkMock->disconnected();
}

TEST_F(ClientTest, keepAliveTimerTimeoutWillSendPingreqPacket)
{
    QMQTT::Frame frame;
    ASSERT_NE(QMQTT::PingreqType, frame._header >> 4);
    EXPECT_CALL(*_networkMock, sendFrame(_)).WillOnce(SaveArg<0>(&frame));

    emit _keepAliveTimerMock->timeout();

    EXPECT_EQ(QMQTT::PingreqType, frame._header >> 4);
}

// todo: connack, connection accepted
// todo: connack, connection refused, unnacceptable protocol
// todo: connack, connection refused, identifier rejected

TEST_F(ClientTest, publishEmitsPublishedSignalWhenQos0_Test)
{
    qRegisterMetaType<QMQTT::Message>("QMQTT::Message&");
    QSignalSpy spy(_client.data(), &QMQTT::Client::published);

    QMQTT::Message message(222, "topic", QByteArray("payload"));
    _client->publish(message);

    ASSERT_EQ(1, spy.count());
    EXPECT_EQ(message, spy.at(0).at(0).value<QMQTT::Message>());
}

// todo: will not be true for Qos1 and Qos2
TEST_F(ClientTest, receivingPublishPacketEmitsReceivedSignal_Test)
{
    QSignalSpy spy(_client.data(), &QMQTT::Client::received);

    emit _networkMock->received(QMQTT::PublishPacket().toFrame());

    EXPECT_EQ(1, spy.count());
}

TEST_F(ClientTest, receivingSubackPacketEmitsSubscribedSignal_Test)
{
    QSignalSpy spy(_client.data(), &QMQTT::Client::subscribed);

    _networkMock->received(QMQTT::SubackPacket().toFrame());

    EXPECT_EQ(1, spy.count());
    // todo: should be able to emit the topic being subscribed
    // Can't yet, since we don't track subscriptions in a session
    //EXPECT_EQ("topic", spy.at(0).at(0).toString());
}

TEST_F(ClientTest, receivingUnsubackPacketEmitsUnsubscribedSignal_Test)
{
    QSignalSpy spy(_client.data(), &QMQTT::Client::unsubscribed);

    QMQTT::UnsubackPacket unsubackPacket;
    emit _networkMock->received(unsubackPacket.toFrame());

    EXPECT_EQ(1, spy.count());
    // todo: should be able to emit the topic being unsubscribed
    // Can't yet, since we don't track subscriptions in a session
    //EXPECT_EQ("topic", spy.at(0).at(0).toString());
}

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
    ASSERT_EQ(1, spy.count());
    EXPECT_EQ(QMQTT::SocketConnectionRefusedError,
              spy.at(0).at(0).value<QMQTT::ClientError>());
}
