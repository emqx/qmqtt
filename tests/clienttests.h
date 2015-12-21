#ifndef CLIENT_TESTS_H
#define CLIENT_TESTS_H

#include "qmqtt_client.h"
#include <QObject>
#include <QSharedPointer>
#include <QTcpServer>

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
    quint16 readInt(QDataStream& in);
    QByteArray readByteArray(QDataStream& in, const quint64 length);
    QString readString(QDataStream& in);

private slots:
    void init();
    void cleanup();

    void constructorWithNoParameters_Test();
    void constructorWithHost_Test();
    void constructorWithHostAndPort_Test();
    void constructorWithHostPortAndParent_Test();
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
    void cleansessReturnsCleansess_Test();
    void setCleansessSetsCleansess_Test();
    void connectWillMakeTCPConnection_Test();
    void isConnectedIsFalseWhenNotConnected_Test();
    void isConnectedIsTrueWhenConnected_Test();
    void autoReconnectDefaultsToFalse_Test();
    void autoReconnectCanBeSetToTrue_Test();
    void autoReconnectDoesNotReconnect_Test();
    void willDefaultsToNull_Test();
    void setWillSetsAWill_Test();
    void stateDefaultsToStateInit_Test();
    void stateStuckOnStateInitEvenWhenConnectedToSocket_Test();
    void publishWillPublishAMessage();
};

#endif // CLIENT_TESTS_H
