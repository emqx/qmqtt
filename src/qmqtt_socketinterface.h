#ifndef QMQTT_SOCKET_INTERFACE_H
#define QMQTT_SOCKET_INTERFACE_H

#include <QHostAddress>
#include <QIODevice>

namespace QMQTT
{

class SocketInterface : public QIODevice
{
    Q_OBJECT
public:
    explicit SocketInterface(QObject* parent = NULL) : QIODevice(parent)
    {
        setOpenMode(QIODevice::ReadWrite);
    }
    virtual	~SocketInterface() {}

    virtual void connectToHost(const QHostAddress& address, quint16 port) = 0;
    virtual void disconnectFromHost() = 0;
    virtual QAbstractSocket::SocketState state() const = 0;
    virtual bool atEnd() const = 0;
    virtual bool waitForBytesWritten(int msecs) = 0;
    virtual QAbstractSocket::SocketError error() const = 0;
    virtual qint64 readData(char* data, qint64 maxlen) = 0;
    virtual qint64 writeData(const char* data, qint64 len) = 0;

signals:
    void connected();
    void disconnected();
    void readyRead();
    void error(QAbstractSocket::SocketError socketError);
};

}

#endif // QMQTT_SOCKET_INTERFACE_H
