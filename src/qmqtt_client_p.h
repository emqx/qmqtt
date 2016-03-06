/*
 * qmqtt_client_p.h - qmqtt client private header
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
#ifndef QMQTT_CLIENT_P_H
#define QMQTT_CLIENT_P_H

#include "qmqtt_client.h"
#include "qmqtt_client_p.h"
#include "qmqtt_publishpacket.h"
#include <QTimer>

namespace QMQTT {

class ClientPrivate
{
public:
    ClientPrivate(Client* qq_ptr);
    ~ClientPrivate();

    void init(const QHostAddress& host, const quint16 port,
              NetworkInterface* network = NULL,
              TimerInterface* pingRespTimer = NULL,
              TimerInterface* keepAliveTimer = NULL);

    QHostAddress _host;
    quint16 _port;
    quint16 _gmid;
    QString _clientId;
    QString _username;
    QString _password;
    bool _cleanSession;
    ConnectionState _connectionState;
    QScopedPointer<NetworkInterface> _network;
    QScopedPointer<TimerInterface> _pingrespTimer;
    QScopedPointer<TimerInterface> _keepAliveTimer;
    QString _willTopic;
    quint8 _willQos;
    bool _willRetain;
    QString _willMessage;
    QHash<QAbstractSocket::SocketError, ClientError> _socketErrorHash;

    Client* const q_ptr;

    QString randomClientId();
    quint16 nextmid();
    void connectToHost();
    void sendPingreqPacket();
    void disconnectFromHost();
    void onNetworkConnected();
    void onNetworkDisconnected();
    quint16 publish(const Message& message);
    quint16 subscribe(const QString& topic, const quint8 qos);
    void unsubscribe(const QString& topic);
    void onNetworkReceived(const Frame& frame);
    void connackPacketReceived(const Frame& frame);
    void publishPacketReceived(const Frame& frame);
    void pubackPacketReceived(const Frame& frame);
    void pubrecPacketReceived(const Frame& frame);
    void pubrelPacketReceived(const Frame& frame);
    void pubcompPacketReceived(const Frame& frame);
    void subackPacketReceived(const Frame& frame);
    void unsubackPacketReceived(const Frame& frame);
    void pingrespPacketReceived(const Frame& frame);
    void disconnectPacketReceived(const Frame& frame);
    bool autoReconnect() const;
    void setAutoReconnect(const bool autoReconnect);
    bool autoReconnectInterval() const;
    void setAutoReconnectInterval(const int autoReconnectInterval);
    bool isConnectedToHost() const;
    QMQTT::ConnectionState connectionState() const;
    void setCleanSession(const bool cleanSession);
    bool cleanSession() const;
    void setKeepAlive(const int keepAlive);
    int keepAlive() const;
    void setPassword(const QString& password);
    QString password() const;
    void setUsername(const QString& username);
    QString username() const;
    void setClientId(const QString& clientId);
    QString clientId() const;
    void setPort(const quint16 port);
    quint16 port() const;
    void setHost(const QHostAddress& host);
    QHostAddress host() const;
    void setWillTopic(const QString& willTopic);
    void setWillQos(const quint8 willQos);
    void setWillRetain(const bool willRetain);
    void setWillMessage(const QString& willMessage);
    QString willTopic() const;
    quint8 willQos() const;
    bool willRetain() const;
    QString willMessage() const;
    void initializeErrorHash();
    void onNetworkError(QAbstractSocket::SocketError error);

    void sendPacket(const AbstractPacket& packet);
    void setConnectionState(const ConnectionState& connectionState);

    Q_DECLARE_PUBLIC(Client)
};

} // namespace QMQTT

#endif // QMQTT_CLIENT_P_H
