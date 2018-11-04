#ifdef QT_WEBSOCKETS_LIB

#include "qmqtt_websocket_p.h"

#include <QNetworkRequest>
#include <QUrl>
#include <QSslError>

QMQTT::WebSocket::WebSocket(const QString& origin,
                            QWebSocketProtocol::Version version,
                            const QSslConfiguration* sslConfig,
                            bool ignoreSelfSigned,
                            QObject* parent)
    : SocketInterface(parent)
    , _socket(new QWebSocket(origin, version, this))
    , _ioDevice(new WebSocketIODevice(_socket, this))
    , _ignoreSelfSigned(ignoreSelfSigned)
{
    if (sslConfig != NULL)
        _socket->setSslConfiguration(*sslConfig);
    connect(_socket, &QWebSocket::connected, this, &WebSocket::connected);
    connect(_socket, &QWebSocket::disconnected, this, &WebSocket::disconnected);
    connect(_socket,
            static_cast<void (QWebSocket::*)(QAbstractSocket::SocketError)>(&QWebSocket::error),
            this,
            static_cast<void (SocketInterface::*)(QAbstractSocket::SocketError)>(&SocketInterface::error));
    connect(_socket, &QWebSocket::sslErrors, this, &WebSocket::sslErrors);
}

QMQTT::WebSocket::~WebSocket()
{
}

void QMQTT::WebSocket::connectToHost(const QHostAddress& address, quint16 port)
{
    Q_UNUSED(address)
    Q_UNUSED(port)
    qFatal("No supported");
}

void QMQTT::WebSocket::connectToHost(const QString& hostName, quint16 port)
{
    Q_UNUSED(port)
    QUrl url(hostName);
    QNetworkRequest request(url);
    request.setRawHeader("Sec-WebSocket-Protocol", "mqtt");
    _ioDevice->connectToHost(request);
}

void QMQTT::WebSocket::disconnectFromHost()
{
    _socket->close();
}

QAbstractSocket::SocketState QMQTT::WebSocket::state() const
{
    return  _socket->state();
}

QAbstractSocket::SocketError QMQTT::WebSocket::error() const
{
    return _socket->error();
}

void QMQTT::WebSocket::sslErrors(const QList<QSslError> &errors)
{
    if (!_ignoreSelfSigned)
        return;
    foreach (QSslError error, errors)
    {
        if (error.error() != QSslError::SelfSignedCertificate &&
            error.error() != QSslError::SelfSignedCertificateInChain)
        {
            return;
        }
    }
    _socket->ignoreSslErrors();
}

#endif // QT_WEBSOCKETS_LIB
