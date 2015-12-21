#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <QTcpServer>
#include <QHostAddress>

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    TcpServer();
    virtual ~TcpServer();

    QByteArray data() const;
    QTcpSocket* socket() const;

protected:
    QTcpSocket* _socket;
    QByteArray _data;

    static const QHostAddress HOST;
    static const quint16 PORT;

protected slots:
    void on_newConnection();
    void on_readyRead();
};

#endif // TCP_SERVER_H