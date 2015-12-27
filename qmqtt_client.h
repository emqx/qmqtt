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

#include "qmqtt_global.h"
#include <QObject>
#include <QAbstractSocket>
#include <QScopedPointer>

namespace QMQTT {

static const quint8 LIBRARY_VERSION_MAJOR = 0;
static const quint8 LIBRARY_VERSION_MINOR = 3;
static const quint8 LIBRARY_VERSION_REVISION = 0;
//static const char* LIBRARY_VERSION = "0.3.0";

static const quint8 PROTOCOL_VERSION_MAJOR = 3;
static const quint8 PROTOCOL_VERSION_MINOR = 1;
static const quint8 PROTOCOL_VERSION_REVISION = 1;
//static const char* PROTOCOL_VERSION = "MQTT/3.1";

enum ClientState
{
    STATE_INIT = 0,
    STATE_CONNECTING,
    STATE_CONNECTED,
    STATE_DISCONNECTED
};

class ClientPrivate;
class Will;
class Message;
class Frame;

class QMQTTSHARED_EXPORT Client : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint32 _port READ port WRITE setPort)
    Q_PROPERTY(QString _host READ host WRITE setHost)
    Q_PROPERTY(QString _clientId READ clientId WRITE setClientId)
    Q_PROPERTY(QString _username READ username WRITE setUsername)
    Q_PROPERTY(QString _password READ password WRITE setPassword)
    Q_PROPERTY(int _keepalive READ keepalive WRITE setKeepAlive)
    Q_PROPERTY(bool _autoReconnect READ autoReconnect WRITE setAutoReconnect)

public:
    Client(const QString host = "localhost", const quint16 port = 1883, QObject* parent = 0);
    ~Client();

    QString host() const;
    quint16 port() const;
    QString clientId() const;
    QString username() const;
    QString password() const;
    int keepalive() const;
    bool cleansess();
    bool isConnectedToHost() const;
    bool autoReconnect() const;
    Will* will() const;
    ClientState state() const;

public slots:
    void setHost(const QString& host);
    void setPort(const quint16 port);
    void setClientId(const QString& clientId);
    void setUsername(const QString& username);
    void setPassword(const QString& password);
    void setKeepAlive(int keepalive);
    void setCleansess(bool cleansess);
    void setAutoReconnect(bool value);
    void setWill(Will *will);

    void connectToHost();
    void disconnectFromHost();

    quint16 publish(Message &message);
    void puback(const quint8 type, const quint16 msgid);
    /*
    void pubrec(int msgid);
    void pubrel(int msgid);
    void pubcomp(int msgid);
    */
    quint16 subscribe(const QString &topic, const quint8 qos);
    void unsubscribe(const QString &topic);
    void ping();

signals:
    void connected();
    void disconnected();
    void error(const QAbstractSocket::SocketError socketError);
    void connacked(const quint8 ack);
    //send PUBLISH and receive PUBACK
    void published(const QMQTT::Message& message);
    void pubacked(const quint8 type, const quint16 msgid);
    //receive PUBLISH
    void received(const QMQTT::Message& message);
    //send SUBSCRIBE and receive SUBACKED
    void subscribed(const QString& topic);
    void subacked(const quint16 mid, const quint8 qos);
    //send UNSUBSCRIBE and receive UNSUBACKED
    void unsubscribed(const QString& topic);
    void unsubacked(const quint16 mid);
    //receive PINGRESP
    void pong();

private slots:
    void onNetworkConnected();
    void onNetworkDisconnected();
    void onReceived(const QMQTT::Frame& frame);

protected:
    QScopedPointer<ClientPrivate> d_ptr;

private:
    Q_DISABLE_COPY(Client)
    Q_DECLARE_PRIVATE(Client)
};

} // namespace QMQTT

#endif // QMQTT_CLIENT_H
