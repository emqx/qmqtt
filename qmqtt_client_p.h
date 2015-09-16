/*
 * qmqtt_client_p.h - qmqtt client private heaer
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

#include <QObject>
#include <QPointer>
#include <QTimer>
#include <QDateTime>
#include <QHostInfo>
#include <QLoggingCategory>

#include "qmqtt_global.h"
#include "qmqtt_message.h"
#include "qmqtt_will.h"
#include "qmqtt_network.h"
namespace QMQTT {

Q_DECLARE_LOGGING_CATEGORY(client)
class ClientPrivate
{

    Q_DECLARE_PUBLIC(Client)
public:
    ClientPrivate(Client * qt);
    ~ClientPrivate();
    void init(QObject * parent = 0);
    void init(const QString & host, int port, QObject *parent = 0);

    QString host;
    quint32 port;
    quint16 gmid;

    QString clientId;
    QString username;
    QString password;
    bool cleansess;
    int keepalive;

    State state;

    QPointer<QMQTT::Will> will;
    QPointer<QMQTT::Network> network;
    QPointer<QTimer> timer;



public slots:
    void sockConnect();
    void sendConnect();
    void sendPing();
    quint16 sendUnsubscribe(const QString &topic);
    quint16 sendSubscribe(const QString &topic, quint8 qos);
    quint16 sendPublish(Message &msg);
    void sendPuback(quint8 type, quint16 mid);
    void sendDisconnect();
    void disconnect();
    void startKeepalive();
    void stopKeepalive();

private:
    QString randomClientId();
    quint16 nextmid();
    Client * const q_ptr;



};

} // namespace QMQTT

#endif // QMQTT_CLIENT_P_H
