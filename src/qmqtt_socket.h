#ifndef QMQTT_SOCKET_H
#define QMQTT_SOCKET_H

#include "qmqtt_socketinterface.h"
#include <QObject>
#include <QScopedPointer>

class QTcpSocket;

namespace QMQTT
{

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
    qint64 write(const char* data, qint64 maxSize);
    bool waitForBytesWritten(int msecs = 30000);
    bool waitForReadyRead(int msecs = 30000);
    QAbstractSocket::SocketError error() const;

    // QIODevice pure virtuals
    qint64 readData(char* data, qint64 maxlen);
    qint64 writeData(const char* data, qint64 len);

    // QIODevice virtuals
    bool atEnd() const;
//    bool open(OpenMode mode);
//    virtual qint64	bytesAvailable() const
//    virtual qint64	bytesToWrite() const
//    virtual bool	canReadLine() const
//    virtual void	close()
//    bool isSequential() const;

protected slots:
    void onConnected();
    void onDisconnected();

protected:
    QScopedPointer<QTcpSocket> _socket;
};

}

#endif // QMQTT_SOCKET_H
