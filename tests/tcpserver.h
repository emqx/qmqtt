#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <QTcpServer>
#include <QHostAddress>

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer();
    virtual ~TcpServer();

    QByteArray data() const;
    QTcpSocket* socket() const;

    static const QHostAddress HOST;
    static const quint16 PORT;

protected:
    QTcpSocket* _socket;
    QByteArray _data;

protected slots:
    void on_newConnection();
    void on_readyRead();
};

#endif // TCP_SERVER_H
