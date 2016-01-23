#ifndef SOCKET_MOCK_H
#define SOCKET_MOCK_H

#include <qmqtt_socketinterface.h>
#include <gmock/gmock.h>

class SocketMock : public QMQTT::SocketInterface
{
public:
//     MOCK_METHOD3(connectToHost, void(const QHostAddress&, quint16, QIODevice::OpenMode));
     MOCK_METHOD2(connectToHost, void(const QHostAddress&, quint16));
     MOCK_METHOD0(disconnectFromHost, void());
     MOCK_CONST_METHOD0(state, QAbstractSocket::SocketState());
     MOCK_CONST_METHOD0(atEnd, bool());
     MOCK_METHOD1(waitForBytesWritten, bool(int));
     MOCK_CONST_METHOD0(error, QAbstractSocket::SocketError());

     MOCK_METHOD2(readData, qint64(char*, qint64));
     MOCK_METHOD2(writeData, qint64(const char *, qint64));
     MOCK_CONST_METHOD0(openMode, QIODevice::OpenMode());

     MOCK_METHOD2(write, qint64(const char*, qint64));
     MOCK_METHOD1(write, qint64(const char*));
     MOCK_METHOD1(write, qint64(const QByteArray&));
};

#endif // SOCKET_MOCK_H
