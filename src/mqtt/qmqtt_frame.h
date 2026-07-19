/*
 * qmqtt_frame.h - qmqtt frame heaer
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
#ifndef QMQTT_FRAME_H
#define QMQTT_FRAME_H

#include <qmqtt_global.h>

#include <QMetaType>
#include <QByteArray>
#include <QString>

QT_FORWARD_DECLARE_CLASS(QDataStream)

constexpr char PROTOCOL_MAGIC_3_1_0[] = "MQIsdp";
constexpr char PROTOCOL_MAGIC_3_1_1[] = "MQTT";

constexpr char RANDOM_CLIENT_PREFIX[] = "QMQTT-";

constexpr quint8 CONNECT     = 0x10;
constexpr quint8 CONNACK     = 0x20;
constexpr quint8 PUBLISH     = 0x30;
constexpr quint8 PUBACK      = 0x40;
constexpr quint8 PUBREC      = 0x50;
constexpr quint8 PUBREL      = 0x60;
constexpr quint8 PUBCOMP     = 0x70;
constexpr quint8 SUBSCRIBE   = 0x80;
constexpr quint8 SUBACK      = 0x90;
constexpr quint8 UNSUBSCRIBE = 0xA0;
constexpr quint8 UNSUBACK    = 0xB0;
constexpr quint8 PINGREQ     = 0xC0;
constexpr quint8 PINGRESP    = 0xD0;
constexpr quint8 DISCONNECT  = 0xE0;

constexpr inline quint8 LSB(quint16 A) { return quint8(A & 0x00FF); }
constexpr inline quint8 MSB(quint16 A) { return quint8((A & 0xFF00) >> 8); }

/*
|--------------------------------------
| 7 6 5 4 |     3    |  2 1  | 0      |
|  Type   | DUP flag |  QoS  | RETAIN |
|--------------------------------------
*/
constexpr inline quint8 GETTYPE  (quint8 HDR)           { return HDR & 0xF0; }
constexpr inline quint8 SETQOS   (quint8 HDR, quint8 Q) { return (HDR & ~0x06) | ((Q & 0x03) << 1); }
constexpr inline quint8 GETQOS   (quint8 HDR)           { return (HDR & 0x06) >> 1; }
constexpr inline quint8 SETDUP   (quint8 HDR, quint8 D) { return (HDR & ~0x08) | ((D & 0x01) << 3); }
constexpr inline quint8 GETDUP   (quint8 HDR)           { return (HDR & 0x08) >> 3; }
constexpr inline quint8 SETRETAIN(quint8 HDR, quint8 R) { return (HDR & ~0x01) | (R & 0x01); }
constexpr inline quint8 GETRETAIN(quint8 HDR)           { return HDR & 0x01; }

/*
|----------------------------------------------------------------------------------
|     7    |    6     |      5     |  4   3  |     2    |       1      |     0    |
| username | password | willretain | willqos | willflag | cleansession | reserved |
|----------------------------------------------------------------------------------
*/
constexpr inline quint8 FLAG_CLEANSESS (quint8 F, quint8 C) { return (F & ~0x02) | ((C & 0x01) << 1); }
constexpr inline quint8 FLAG_WILL      (quint8 F, quint8 W) { return (F & ~0x04) | ((W & 0x01) << 2); }
constexpr inline quint8 FLAG_WILLQOS   (quint8 F, quint8 Q) { return (F & ~0x18) | ((Q & 0x03) << 3); }
constexpr inline quint8 FLAG_WILLRETAIN(quint8 F, quint8 R) { return (F & ~0x20) | ((R & 0x01) << 5); }
constexpr inline quint8 FLAG_PASSWD    (quint8 F, quint8 P) { return (F & ~0x40) | ((P & 0x01) << 6); }
constexpr inline quint8 FLAG_USERNAME  (quint8 F, quint8 U) { return (F & ~0x80) | ((U & 0x01) << 7); }

namespace QMQTT {

class Q_MQTT_EXPORT Frame
{
public:
    explicit Frame();
    explicit Frame(const quint8 header);
    explicit Frame(const quint8 header, const QByteArray &data);
    virtual ~Frame();

    Frame(const Frame& other);
    Frame& operator=(const Frame& other);

    bool operator==(const Frame& other) const;
    inline bool operator!=(const Frame& other) const
    { return !operator==(other); }

    quint8 header() const;
    QByteArray data() const;

    quint16 readInt();
    quint8 readChar();
    QByteArray readByteArray();
    QString readString();

    void writeInt(const quint16 i);
    void writeChar(const quint8 c);
    void writeByteArray(const QByteArray &data);
    void writeString(const QString &string);
    void writeRawData(const QByteArray &data);

    //TODO: FIXME LATER
    void write(QDataStream &stream) const;
    bool encodeLength(QByteArray &lenbuf, int length) const;

private:
    quint8 _header;
    QByteArray _data;
    int _readOffset;
};

} // namespace QMQTT

Q_DECLARE_METATYPE(QMQTT::Frame)

#endif // QMQTT_FRAME_H
