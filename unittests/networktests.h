#ifndef NETWORK_TESTS_H
#define NETWORK_TESTS_H

#include <QObject>
#include <QScopedPointer>
#include <qmqtt_frame.h>

namespace QMQTT
{
    class Network;
}

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

#endif // NETWORK_TESTS_H
