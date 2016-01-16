#ifndef SOCKET_H
#define SOCKET_H

#include <QObject>
#include <QScopedPointer>
#include <QHostAddress>
#include <QIODevice>

class QTcpSocket;

class Socket : public QObject
{
    Q_OBJECT
public:
    explicit Socket(QObject* parent = NULL);
    virtual	~Socket();

    void connectToHost(const QHostAddress & address, quint16 port, QIODevice::OpenMode openMode = QIODevice::ReadWrite);
    void disconnectFromHost();
    QByteArray readAll();
    QAbstractSocket::SocketState state() const;
    bool atEnd() const;

    qint64 write(const char* data, qint64 maxSize);
    bool waitForBytesWritten(int msecs);
    QAbstractSocket::SocketError error() const;

protected:
    QScopedPointer<QTcpSocket> _socket;

signals:
    void connected();
    void disconnected();
    void readyRead();
    void error(QAbstractSocket::SocketError socketError);
};

#endif // SOCKET_H
