#ifndef QMQTT_SOCKET_H
#define QMQTT_SOCKET_H

#include "qmqtt_socketinterface.h"
#include <QObject>
#include <QScopedPointer>

class QTcpSocket;

class Socket : public SocketInterface
{
    Q_OBJECT
public:
    explicit Socket(QObject* parent = NULL);
    virtual	~Socket();

    void connectToHost(const QHostAddress& address, quint16 port, QIODevice::OpenMode openMode = QIODevice::ReadWrite);
    void disconnectFromHost();
    QByteArray readAll();
    QAbstractSocket::SocketState state() const;
    bool atEnd() const;
    qint64 write(const char* data, qint64 maxSize);
    bool waitForBytesWritten(int msecs);
    QAbstractSocket::SocketError error() const;

protected:
    QScopedPointer<QTcpSocket> _socket;
};

#endif // QMQTT_SOCKET_H
