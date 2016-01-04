#ifndef NETWORK_MOCK_H
#define NETWORK_MOCK_H

#include <qmqtt_networkinterface.h>
#include <gmock/gmock.h>

class NetworkMock : public QMQTT::NetworkInterface
{
public:
    MOCK_METHOD1(sendFrame, void(QMQTT::Frame&));
    MOCK_CONST_METHOD0(isConnectedToHost, bool());
    MOCK_CONST_METHOD0(autoReconnect, bool());
    MOCK_METHOD1(setAutoReconnect, void(const bool));
    MOCK_CONST_METHOD0(state, QAbstractSocket::SocketState());
    MOCK_METHOD2(connectToHost, void(const QHostAddress&, const quint16));
    MOCK_METHOD0(disconnectFromHost, void());    
};

#endif // NETWORK_MOCK_H

