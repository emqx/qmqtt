/*
 * qmqtt_client_p.cpp - qmqtt client private
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

#include "qmqtt_client.h"
#include "qmqtt_client_p.h"

namespace QMQTT {

Q_LOGGING_CATEGORY(client, "qmqtt.client")

ClientPrivate::ClientPrivate(Client *qt) :
    host("localhost"),
    port(1883),
    keepalive(300),
    q_ptr(qt)
{
    gmid= 1;
}

ClientPrivate::~ClientPrivate()
{

}

void ClientPrivate::init(QObject * parent)
{
    Q_Q(Client);
    q->setParent(parent);
    if(!timer) {
        timer = new QTimer(q);
    }
    QObject::connect(timer, SIGNAL(timeout()), q, SLOT(ping()));
    if(!network){
        network = new Network(q);
    }
    //TODO: FIXME LATER, how to handle socket error?
    QObject::connect(network, SIGNAL(connected()), q, SLOT(onConnected()));
    QObject::connect(network, SIGNAL(error(QAbstractSocket::SocketError)), q, SIGNAL(error(QAbstractSocket::SocketError)));
    QObject::connect(network, SIGNAL(disconnected()), q, SLOT(onDisconnected()));
    QObject::connect(network, SIGNAL(received(Frame &)), q, SLOT(onReceived(Frame &)));
}

void ClientPrivate::init(const QString &host, int port, QObject * parent)
{
    this->host = host;
    this->port = port;
    init(parent);
}

void ClientPrivate::sockConnect()
{
    network->connectTo(host, port);
}

void ClientPrivate::sendConnect()
{
    QString magic(PROTOCOL_MAGIC);
    quint8 header = CONNECT;
    quint8 flags = 0;

    qCDebug(client) << "sendConnect....";

    //header
    Frame frame(SETQOS(header, MQTT_QOS1));

    qCDebug(client) << "CONNECT header: " << frame.header();

    //flags
    flags = FLAG_CLEANSESS(flags, cleansess ? 1 : 0 );
    flags = FLAG_WILL(flags, will ? 1 : 0);
    if (will) {
        flags = FLAG_WILLQOS(flags, will->qos());
        flags = FLAG_WILLRETAIN(flags, will->retain() ? 1 : 0);
    }
    if (!username.isEmpty()) {
        flags = FLAG_USERNAME(flags, 1);
    }
    if (!password.isEmpty()) {
        flags = FLAG_PASSWD(flags, 1);
    }

    //payload
    frame.writeString(magic);
    frame.writeChar(MQTT_PROTO_MAJOR);
    frame.writeChar(flags);
    frame.writeInt(keepalive);
    if(clientId.isEmpty()) {
        clientId = randomClientId();
    }
    frame.writeString(clientId);
    if(will != NULL) {
        frame.writeString(will->topic());
        frame.writeString(will->message());
    }
    if (!username.isEmpty()) {
        frame.writeString(username);
    }
    if (!password.isEmpty()) {
        frame.writeString(password);
    }
    network->sendFrame(frame);
}

quint16 ClientPrivate::sendPublish(Message &msg)
{
    quint8 header = PUBLISH;
    header = SETRETAIN(header, msg.retain() ? 1 : 0);
    header = SETQOS(header, msg.qos());
    header = SETDUP(header, msg.dup() ? 1 : 0);
    Frame frame(header);
    frame.writeString(msg.topic());
    if(msg.qos() > MQTT_QOS0) {
        if(msg.id() == 0) {
            msg.setId(gmid++);
        }
        frame.writeInt(msg.id());
    }
    if(!msg.payload().isEmpty()) {
        frame.writeRawData(msg.payload());
    }
    network->sendFrame(frame);
    return msg.id();
}

void ClientPrivate::sendPuback(quint8 type, quint16 mid)
{
    Frame frame(type);
    frame.writeInt(mid);
    network->sendFrame(frame);
}

quint16 ClientPrivate::sendSubscribe(const QString & topic, quint8 qos)
{
    quint16 mid = nextmid();
    Frame frame(SETQOS(SUBSCRIBE, MQTT_QOS1));
    frame.writeInt(mid);
    frame.writeString(topic);
    frame.writeChar(qos);
    network->sendFrame(frame);
    return mid;
}

quint16 ClientPrivate::sendUnsubscribe(const QString &topic)
{
    quint16 mid = gmid++;
    Frame frame(SETQOS(UNSUBSCRIBE, MQTT_QOS1));
    frame.writeInt(mid);
    frame.writeString(topic);
    network->sendFrame(frame);
    return mid;
}

void ClientPrivate::sendPing()
{
    Frame frame(PINGREQ);
    network->sendFrame(frame);
}

void ClientPrivate::disconnect()
{
    sendDisconnect();
    network->disconnect();
}

void ClientPrivate::sendDisconnect()
{
    Frame frame(DISCONNECT);
    network->sendFrame(frame);
}

//FIXME: ok???
void ClientPrivate::startKeepalive()
{
    timer->setInterval(keepalive*1000);
    timer->start();
}

void ClientPrivate::stopKeepalive()
{
    timer->stop();
}

QString ClientPrivate::randomClientId()
{
    return "QMQTT-" + QString::number(QDateTime::currentMSecsSinceEpoch() % 1000000);
}

quint16 ClientPrivate::nextmid()
{
    return gmid++;
}

} // namespace QMQTT
