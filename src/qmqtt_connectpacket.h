/*
 * qmqtt_connectdpacket.h - qmqtt connect packet header
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
#ifndef QMQTT_CONNECT_PACKET_H
#define QMQTT_CONNECT_PACKET_H

#include "qmqtt_abstractpacket.h"
#include <QDataStream>
#include <QString>

namespace QMQTT
{

class ConnectPacket : public AbstractPacket
{
    friend QDataStream& operator>>(QDataStream& stream, ConnectPacket& packet);
    friend QDataStream& operator<<(QDataStream& stream, const ConnectPacket& packet);

public:
    ConnectPacket();
    virtual ~ConnectPacket();

    QString protocol() const;
    quint8 protocolLevel() const;

    bool cleanSession() const;
    QString willTopic() const;
    QString willMessage() const;
    quint8 willQos() const;
    bool willRetain() const;
    QString clientIdentifier() const;
    QString userName() const;
    QString password() const;

    void setCleanSession(const bool cleanSession);
    void setWillTopic(const QString& willTopic);
    void setWillMessage(const QString& willMessage);
    void setWillQos(const quint8 willQos);
    void setWillRetain(const bool willRetain);
    void setClientIdentifier(const QString& clientIdentifier);
    void setUserName(const QString& userName);
    void setPassword(const QString& password);

    bool isValid() const;
    PacketType type() const;

protected:
    QString _protocol;
    quint8 _protocolLevel;
    quint8 _connectFlags;
    QString _willTopic;
    QString _willMessage;
    quint16 _keepAlive;
    QString _clientIdentifier;
    QString _userName;
    QString _password;

    bool willFlag() const;
    bool userNameFlag() const;
    bool passwordFlag() const;
    qint64 calculateRemainingLengthFromData() const;
};

QDataStream& operator>>(QDataStream& stream, ConnectPacket& packet);
QDataStream& operator<<(QDataStream& stream, const ConnectPacket& packet);

} // end namespace QMQTT

#endif // QMQTT_CONNECT_PACKET_H
