/*
 * qmqtt_client.h - qmqtt client header
 *
 * Copyright (c) 2013  Ery Lee <ery.lee at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of mqttc nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef QMQTT_CLIENT_H
#define QMQTT_CLIENT_H

#if defined(QMQTT_LIBRARY)
#  define QMQTTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QMQTTSHARED_EXPORT Q_DECL_IMPORT
#endif

#include <QObject>
#include <QAbstractSocket>
#include <QScopedPointer>
#include <QHostAddress>

namespace QMQTT {

static const quint8 LIBRARY_VERSION_MAJOR = 0;
static const quint8 LIBRARY_VERSION_MINOR = 3;
static const quint8 LIBRARY_VERSION_REVISION = 0;
//static const char* LIBRARY_VERSION = "0.3.0";

static const quint8 PROTOCOL_VERSION_MAJOR = 3;
static const quint8 PROTOCOL_VERSION_MINOR = 1;
static const quint8 PROTOCOL_VERSION_REVISION = 1;
//static const char* PROTOCOL_VERSION = "MQTT/3.1";

enum ConnectionState
{
    STATE_INIT = 0,
    STATE_CONNECTING,
    STATE_CONNECTED,
    STATE_DISCONNECTED
};

enum ClientError
{
};

class ClientPrivate;
class Message;
class Frame;
class NetworkInterface;

class QMQTTSHARED_EXPORT Client : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint32 _port READ port WRITE setPort)
    Q_PROPERTY(QHostAddress _host READ host WRITE setHost)
    Q_PROPERTY(QString _clientId READ clientId WRITE setClientId)
    Q_PROPERTY(QString _username READ username WRITE setUsername)
    Q_PROPERTY(QString _password READ password WRITE setPassword)
    Q_PROPERTY(int _keepAlive READ keepAlive WRITE setKeepAlive)
    Q_PROPERTY(bool _autoReconnect READ autoReconnect WRITE setAutoReconnect)
    Q_PROPERTY(bool _cleanSession READ cleanSession WRITE setCleanSession)
    Q_PROPERTY(QString _willTopic READ willTopic WRITE setWillTopic)
    Q_PROPERTY(quint8 _willQos READ willQos WRITE setWillQos)
    Q_PROPERTY(bool _willRetain READ willRetain WRITE setWillRetain)
    Q_PROPERTY(QString _willMessage READ willMessage WRITE setWillMessage)
    Q_PROPERTY(QString _connectionState READ connectionState)

public:
    Client(const QHostAddress& host = QHostAddress::LocalHost,
           const quint16 port = 1883,
           QObject* parent = NULL);

    // for testing purposes only
    Client(NetworkInterface* network,
           const QHostAddress& host = QHostAddress::LocalHost,
           const quint16 port = 1883,
           QObject* parent = NULL);

    virtual ~Client();

    QHostAddress host() const;
    quint16 port() const;
    QString clientId() const;
    QString username() const;
    QString password() const;
    int keepAlive() const;
    bool cleanSession() const;
    bool autoReconnect() const;
    ConnectionState connectionState() const;
    QString willTopic() const;
    quint8 willQos() const;
    bool willRetain() const;
    QString willMessage() const;

    bool isConnectedToHost() const;

public slots:
    void setHost(const QHostAddress& host);
    void setPort(const quint16 port);
    void setClientId(const QString& clientId);
    void setUsername(const QString& username);
    void setPassword(const QString& password);
    void setKeepAlive(int keepAlive);
    void setCleanSession(const bool cleansess);
    void setAutoReconnect(bool value);
    void setWillTopic(const QString& willTopic);
    void setWillQos(const quint8 willQos);
    void setWillRetain(const bool willRetain);
    void setWillMessage(const QString& willMessage);

    void connectToHost();
    void disconnectFromHost();

    quint16 subscribe(const QString& topic, const quint8 qos);
    void unsubscribe(const QString& topic);

    quint16 publish(const Message& message);

signals:
    void connected();
    void disconnected();

    // for pending MQTT protocol errors
    void error(const QMQTT::ClientError error);

    // todo: should emit on server suback (or is that only at specific QoS levels?)
    void subscribed(const QString& topic);
    // todo: should emit on server unsuback (or is that only at specific QoS levels?)
    void unsubscribed(const QString& topic);
    // todo: should emit on server puback (or is that only at specific QoS levels?)
    void published(const QMQTT::Message& message);

    void received(const QMQTT::Message& message);

protected slots:
    void onNetworkConnected();
    void onNetworkDisconnected();
    void onNetworkReceived(const QMQTT::Frame& frame);
    void onTimerPingReq();

protected:
    QScopedPointer<ClientPrivate> d_ptr;

private:
    Q_DISABLE_COPY(Client)
    Q_DECLARE_PRIVATE(Client)
};

} // namespace QMQTT

#endif // QMQTT_CLIENT_H
