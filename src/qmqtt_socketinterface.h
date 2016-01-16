#ifndef QMQTT_SOCKET_INTERFACE_H
#define QMQTT_SOCKET_INTERFACE_H

#include <QObject>
#include <QHostAddress>
#include <QIODevice>

class SocketInterface : public QObject
{
    Q_OBJECT
public:
    explicit SocketInterface(QObject* parent = NULL) : QObject(parent) {}
    virtual	~SocketInterface() {}

    virtual void connectToHost(const QHostAddress& address, quint16 port, QIODevice::OpenMode openMode = QIODevice::ReadWrite) = 0;
    virtual void disconnectFromHost() = 0;
    virtual QByteArray readAll() = 0;
    virtual QAbstractSocket::SocketState state() const = 0;
    virtual bool atEnd() const = 0;
    virtual qint64 write(const char* data, qint64 maxSize) = 0;
    virtual bool waitForBytesWritten(int msecs) = 0;
    virtual QAbstractSocket::SocketError error() const = 0;

signals:
    void connected();
    void disconnected();
    void readyRead();
    void error(QAbstractSocket::SocketError socketError);
};

#endif // QMQTT_SOCKET_INTERFACE_H
