#include "qmqtt_socket.h"
#include <QTcpSocket>

Socket::Socket(QObject* parent)
    : SocketInterface(parent)
    , _socket(new QTcpSocket)
{
    connect(_socket.data(), &QTcpSocket::connected,    this, &SocketInterface::connected);
    connect(_socket.data(), &QTcpSocket::disconnected, this, &SocketInterface::disconnected);
    connect(_socket.data(), &QTcpSocket::readyRead,    this, &SocketInterface::readyRead);
    connect(_socket.data(),
            static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error),
            this,
            static_cast<void (SocketInterface::*)(QAbstractSocket::SocketError)>(&SocketInterface::error));
}

Socket::~Socket()
{
}

void Socket::connectToHost(const QHostAddress& address, quint16 port, QIODevice::OpenMode openMode)
{
    return _socket->connectToHost(address, port, openMode);
}

void Socket::disconnectFromHost()
{
    _socket->disconnectFromHost();
}

QByteArray Socket::readAll()
{
    return _socket->readAll();
}

QAbstractSocket::SocketState Socket::state() const
{
    return _socket->state();
}

bool Socket::atEnd() const
{
    return _socket->atEnd();
}

qint64 Socket::write(const char* data, qint64 maxSize)
{
    return _socket->write(data, maxSize);
}

bool Socket::waitForBytesWritten(int msecs)
{
    return _socket->waitForBytesWritten(msecs);
}

QAbstractSocket::SocketError Socket::error() const
{
    return _socket->error();
}

// QIODevice demands
qint64 Socket::readData(char* data, qint64 maxlen)
{
    return _socket->read(data, maxlen);
}

qint64 Socket::writeData(const char* data, qint64 len)
{
    return _socket->write(data, len);
}
