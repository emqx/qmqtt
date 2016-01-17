#include "qmqtt_socket.h"
#include <QTcpSocket>

QMQTT::Socket::Socket(QObject* parent)
    : SocketInterface(parent)
    , _socket(new QTcpSocket)
{
    connect(_socket.data(), &QTcpSocket::connected,    this, &Socket::onConnected);
    connect(_socket.data(), &QTcpSocket::disconnected, this, &Socket::onDisconnected);

//    connect(_socket.data(), &QTcpSocket::connected,    this, &SocketInterface::connected);
//    connect(_socket.data(), &QTcpSocket::disconnected, this, &SocketInterface::disconnected);
    connect(_socket.data(), &QTcpSocket::readyRead,    this, &SocketInterface::readyRead);
    connect(_socket.data(),
            static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error),
            this,
            static_cast<void (SocketInterface::*)(QAbstractSocket::SocketError)>(&SocketInterface::error));

}

QMQTT::Socket::~Socket()
{
}

void QMQTT::Socket::onConnected()
{
    setOpenMode(_socket->openMode());
    emit connected();
}

void QMQTT::Socket::onDisconnected()
{
    setOpenMode(_socket->openMode());
    emit disconnected();
}

void QMQTT::Socket::connectToHost(const QHostAddress& address, quint16 port, QIODevice::OpenMode openMode)
{
    return _socket->connectToHost(address, port, openMode);
}

void QMQTT::Socket::disconnectFromHost()
{
    _socket->disconnectFromHost();
}

QByteArray QMQTT::Socket::readAll()
{
    return _socket->readAll();
}

QAbstractSocket::SocketState QMQTT::Socket::state() const
{
    return _socket->state();
}

bool QMQTT::Socket::atEnd() const
{
    return _socket->atEnd();
}

qint64 QMQTT::Socket::write(const char* data, qint64 maxSize)
{
    qDebug() << Q_FUNC_INFO;
    return _socket->write(data, maxSize);
}

bool QMQTT::Socket::waitForBytesWritten(int msecs)
{
    return _socket->waitForBytesWritten(msecs);
}

bool QMQTT::Socket::waitForReadyRead(int msecs)
{
    return _socket->waitForReadyRead(msecs);
}

QAbstractSocket::SocketError QMQTT::Socket::error() const
{
    return _socket->error();
}

// QIODevice demands
qint64 QMQTT::Socket::readData(char* data, qint64 maxlen)
{
    return _socket->read(data, maxlen);
}

qint64 QMQTT::Socket::writeData(const char* data, qint64 len)
{
    return _socket->write(data, len);
}

//bool QMQTT::Socket::open(OpenMode mode)
//{
//    return _socket->open(mode);
//}

//bool QMQTT::Socket::isSequential() const
//{
//    return _socket->isSequential();
//}
