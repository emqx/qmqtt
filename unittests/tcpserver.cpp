#include "tcpserver.h"
#include <QTcpSocket>
#include <QDebug>

const quint16 TcpServer::PORT = 3875;

TcpServer::TcpServer()
    : _socket(NULL)
{
    connect(this, &QTcpServer::newConnection, this, &TcpServer::on_newConnection);
    listen(QHostAddress::LocalHost, 3875);
}

TcpServer::~TcpServer()
{
}

void TcpServer::on_newConnection()
{
    if(NULL != _socket)
    {
        disconnect(_socket, &QTcpSocket::readyRead, this, &TcpServer::on_readyRead);
    }
    _socket = nextPendingConnection();
    if(NULL != _socket)
    {
        connect(_socket, &QTcpSocket::readyRead, this, &TcpServer::on_readyRead);
    }
}

void TcpServer::on_readyRead()
{
    _data.append(_socket->readAll());
}

QByteArray TcpServer::data() const
{
    return _data;
}

QTcpSocket* TcpServer::socket() const
{
    return _socket;
}
