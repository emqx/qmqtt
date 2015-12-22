#include "tcpserver.h"
#include <QHostAddress>
#include <QTcpSocket>

const QHostAddress TcpServer::HOST = QHostAddress::LocalHost;
const quint16 TcpServer::PORT = 3875;

TcpServer::TcpServer()
    : _socket(NULL)
{
    connect(this, &QTcpServer::newConnection, this, &TcpServer::on_newConnection);
    listen(HOST, PORT);
}

TcpServer::~TcpServer()
{
}

void TcpServer::on_newConnection()
{
    if(NULL != _socket)
    {
        disconnect(_socket, &QTcpSocket::readyRead, this, &TcpServer::on_readyRead);
        _socket->disconnectFromHost();
        _socket->deleteLater();
        _socket = NULL;
        _data.clear();
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
