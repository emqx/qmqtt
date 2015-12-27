/*
 * qmqtt_network.h - qmqtt network header
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
#ifndef QMQTT_NETWORK_H
#define QMQTT_NETWORK_H

#include <QObject>
#include <QTcpSocket>
#include <QPointer>
#include <QBuffer>
#include <QByteArray>

#include "qmqtt_frame.h"

namespace QMQTT {

class Network : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Network)
public:
    explicit Network(QObject *parent = 0);
    ~Network();

    void disconnect();
    void sendFrame(Frame & frame);

    bool isConnected();

    bool autoReconnect() const;
    void setAutoReconnect(bool value);

    QAbstractSocket::SocketState state() const;

signals:
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError);
    void received(Frame &frame);

public slots:
    void connectTo(const QString & host, quint32 port);
    //void error( QAbstractSocket::SocketError socketError );

private slots:
    void sockReadReady();
    void sockConnected();
    //TODO: FIX LATER, add reconnect feature
    //void sockError(QAbstractSocket::SocketError);
    void sockDisconnected();

private:
    void initSocket();
    int readRemaingLength(QDataStream &in);
    //sock
    quint32 _port;
    QString _host;
    QPointer<QTcpSocket> _socket;
    //read data
    QPointer<QBuffer> _buffer;
    quint8 _header;
    int _offsetBuf;
    int _leftSize;
    //autoconn
    bool _autoreconn;
    quint16 _timeout;
    //state
    bool _connected;
};

} // namespace QMQTT

#endif // QMQTT_NETWORK_H
