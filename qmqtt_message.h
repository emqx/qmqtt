/*
 * qmqtt_message.h - qmqtt message header
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
#ifndef QMQTT_MESSAGE_H
#define QMQTT_MESSAGE_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include "qmqtt_global.h"
namespace QMQTT {

class QMQTTSHARED_EXPORT Message
{
public:
    Message();
    Message(quint16 id, const QString &topic, const QByteArray &payload,
            quint8 qos = 0, bool retain = false, bool dup = false);
    ~Message();

    quint16 id();
    void setId(quint16 id);

    quint8 qos() const;
    void setQos(quint8 qos);

    bool retain();
    void setRetain(bool retain);

    bool dup();
    void setDup(bool dup);

    QString topic() const;
    void setTopic(const QString &topic);

    QByteArray payload() const;
    void setPayload(const QByteArray & payload);

private:
    quint16 _id;
    QString _topic;
    QByteArray _payload;
    quint8 _qos;
    bool _retain;
    bool _dup;

};

} // namespace QMQTT

#endif // QMQTT_MESSAGE_H
