/*
 * qmqtt_client.h - qmqtt client heaer
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

#include <QObject>
#include "qmqtt_global.h"
#include "qmqtt_will.h"
#include "qmqtt_message.h"
#include "qmqtt_network.h"

/*
 * MQTT Version
 */
#define MQTT_PROTO_MAJOR 3
#define MQTT_PROTO_MINOR 1
#define MQTT_PROTOCOL_VERSION "MQTT/3.1"

/*
 * MQTT QOS
 */
#define MQTT_QOS0 0
#define MQTT_QOS1 1
#define MQTT_QOS2 2

namespace QMQTT {

/*
 * MQTT ConnAck
 */
enum ConnAck
{
    CONNACK_ACCEPT  = 0,
    CONNACK_PROTO_VER,
    CONNACK_INVALID_ID,
    CONNACK_SERVER,
    CONNACK_CREDENTIALS,
    CONNACK_AUTH
};

enum State
{
    STATE_INIT = 0,
    STATE_CONNECTING,
    STATE_CONNECTED,
    STATE_DISCONNECTED
};

class ClientPrivate;
class QMQTTSHARED_EXPORT Client : public QObject
{
    Q_OBJECT

    Q_PROPERTY(quint32 port READ port WRITE setPort)
    Q_PROPERTY(QString host READ host WRITE setHost)
    Q_PROPERTY(QString clientId READ clientId WRITE setClientId)
    Q_PROPERTY(QString username READ username WRITE setUsername)
    Q_PROPERTY(QString password READ password WRITE setPassword)
    Q_PROPERTY(int keepalive READ keepalive WRITE setKeepAlive)
    Q_PROPERTY(bool autoReconnect READ autoReconnect WRITE setAutoReconnect)


//    friend class ClientPrivate;

public:
    Client(const QString &host = "localhost", quint32 port = 1883, QObject * parent = 0);
    ~Client();

    /*
     * Property Get/Set
     */
    QString host() const;
    void setHost(const QString & host);
    quint32 port() const;
    void setPort(quint32 port);

    QString clientId() const;
    void setClientId(const QString &clientId);
    QString username() const;
    void setUsername(const QString & username);
    QString password() const;
    void setPassword(const QString & password);
    int keepalive();
    void setKeepAlive(int keepalive);
    bool cleansess();
    void setCleansess(bool cleansess);

    bool isConnected();
    bool autoReconnect() const;
    void setAutoReconnect(bool value);
    Will *will();
    void setWill(Will *will);

    State state() const;

    /*
     * MQTT Command
     */
public slots:
    void connect();
    quint16 publish(Message &message);
    void puback(quint8 type, quint16 msgid);
    /*
    void pubrec(int msgid);
    void pubrel(int msgid);
    void pubcomp(int msgid);
    */
    quint16 subscribe(const QString &topic, quint8 qos);
    void unsubscribe(const QString &topic);
    void ping();
    void disconnect();

signals:
    void connected();
    void error(QAbstractSocket::SocketError);
    void connacked(quint8 ack);
    //send PUBLISH and receive PUBACK
    void published(QMQTT::Message &message);
    void pubacked(quint8 type, quint16 msgid);
    //receive PUBLISH
    void received(const QMQTT::Message &message);
    //send SUBSCRIBE and receive SUBACKED
    void subscribed(const QString &topic);
    void subacked(quint16 mid, quint8 qos);
    //send UNSUBSCRIBE and receive UNSUBACKED
    void unsubscribed(const QString &topic);
    void unsubacked(quint16 mid);
    //receive PINGRESP
    void pong();
    void disconnected();

private slots:
    void onConnected();
    void onDisconnected();
    void onReceived(Frame & frame);
    void handlePublish(Message &message);
    void handleConnack(quint8 ack);
    void handlePuback(quint8 type, quint16 msgid);

private:
    ClientPrivate *const  d_ptr;

    Q_DISABLE_COPY(Client)
    Q_DECLARE_PRIVATE(Client)

};

} // namespace QMQTT
#endif // QMQTT_CLIENT_H
