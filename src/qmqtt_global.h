/*
 * qmqtt_global.h - qmqtt libray global
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
#ifndef QMQTT_GLOBAL_H
#define QMQTT_GLOBAL_H

#if defined(QMQTT_LIBRARY)
#  define QMQTTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QMQTTSHARED_EXPORT Q_DECL_IMPORT
#endif

#include <qglobal.h>
#include <QString>

namespace QMQTT
{

enum PacketType
{
    ConnectType     = 0x01,
    ConnackType     = 0x02,
    PublishType     = 0x03,
    PubackType      = 0x04,
    PubrecType      = 0x05,
    PubrelType      = 0x06,
    PubcompType     = 0x07,
    SubscribeType   = 0x08,
    SubackType      = 0x09,
    UnsubscribeType = 0x0a,
    UnsubackType    = 0x0b,
    PingreqType     = 0x0c,
    PingrespType    = 0x0d,
    DisconnectType  = 0x0e
};

enum QosType
{
    Qos0 = 0x00,
    Qos1 = 0x01,
    Qos2 = 0x02
};

enum ConnectReturnCodeType
{
    ConnectionAccepted = 0x00,
    ConnectionRefusedUnacceptableProtocolVersion = 0x01,
    ConnectionRefusedIdentifierRejected = 0x02,
    ConnectionRefusedServerUnavailable = 0x03,
    ConnectionRefusedBadUserNameOrPassword = 0x04,
    ConnectionRefusedNotAuthorized = 0x05
};

class Subscription
{
public:
    QString _topicFilter;
    QosType _requestedQos;
};

typedef QList<Subscription> SubscriptionList;

enum SubackReturnCodeType
{
    SubackSuccessMaximumQoS0 = 0x00,
    SubackSuccessMaximumQoS1 = 0x01,
    SubackSuccessMaximumQoS2 = 0x02,
    SubackFailure = 0x80
};

typedef QList<SubackReturnCodeType> SubackReturnCodeList;

} // namespace QMQTT

#endif // QMQTT_GLOBAL_H

