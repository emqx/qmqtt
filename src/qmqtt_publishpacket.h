/*
 * qmqtt_publishpacket.h - qmqtt publish packet header
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
#ifndef QMQTT_PUBLISH_PACKET_H
#define QMQTT_PUBLISH_PACKET_H

#include "qmqtt_abstractpacket.h"
#include <QDataStream>
//#include <qglobal.h>
#include <QString>

namespace QMQTT
{

class PublishPacket : public AbstractPacket
{
public:
    PublishPacket();
    virtual ~PublishPacket();

    bool isValid() const;
    PacketType type() const;

    bool dupFlag() const;
    void setDupFlag(const bool dupFlag);
    quint8 qos() const;
    void setQos(const quint8 qos);
    bool retainFlag() const;
    void setRetainFlag(const bool retainFlag);
    QString topicName() const;
    void setTopicName(const QString& topicName);
    quint16 packetIdentifier() const;
    void setPacketIdentifier(const quint16 packetIdentifier);
    QByteArray payload() const;
    void setPayload(const QByteArray& payload);

protected:
    QString _topicName;
    quint16 _packetIdentifier;
    QByteArray _payload;

    qint64 calculateRemainingLengthFromData() const;

private:
    friend QDataStream& operator>>(QDataStream& stream, PublishPacket& packet);
    friend QDataStream& operator<<(QDataStream& stream, const PublishPacket& packet);
};

QDataStream& operator>>(QDataStream& stream, PublishPacket& packet);
QDataStream& operator<<(QDataStream& stream, const PublishPacket& packet);

} // end namespace QMQTT

#endif // QMQTT_PUBLISH_PACKET_H
