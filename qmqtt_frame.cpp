/*
 * qmqtt_frame.cpp - qmqtt frame
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
#include <QDataStream>
#include <QLoggingCategory>
#include "qmqtt_frame.h"

namespace QMQTT {

Q_LOGGING_CATEGORY(frame, "qmqtt.frame")

Frame::Frame(quint8 header, QObject *parent) :
    QObject(parent),
    _header(header),
    _data(QByteArray())
{
}

Frame::Frame(quint8 header, QByteArray & data, QObject *parent) :
    QObject(parent),
    _header(header),
    _data(data)
{
}

Frame::~Frame()
{

}

quint8 Frame::header()
{
    return _header;
}

QByteArray & Frame::data()
{
    return _data;
}

char Frame::readChar()
{
    char c = _data.at(0);
    _data.remove(0, 1);
    return c;
}

int Frame::readInt()
{
    char msb = _data.at(0);
    char lsb = _data.at(1);
    _data.remove(0, 2);
    return (msb << 8) + lsb;
}

QString Frame::readString()
{
    int len = readInt();
    QString s(_data.left(len));
    _data.remove(0, len);
    return s;
}

void Frame::writeInt(int i)
{
    _data.append(MSB(i));
    _data.append(LSB(i));
}

void Frame::writeString(const QString &string)
{
    writeInt(string.size());
    _data.append(string);
}

void Frame::writeChar(char c)
{
    _data.append(c);
}

void Frame::writeRawData(const QByteArray &data)
{
    _data.append(data);
}

void Frame::write(QDataStream &stream)
{
    QByteArray lenbuf;
    stream << (quint8)_header;
    if(_data.size() == 0) {
        stream << (quint8)0;
        return;
    }
    qCDebug(frame) << "_data.size: %d" << _data.size();
    encodeLength(lenbuf, _data.size());
    stream.writeRawData(lenbuf.data(), lenbuf.size());
    stream.writeRawData(_data.data(), _data.size());
}

void Frame::encodeLength(QByteArray &lenbuf, int length)
{
    char d;
    do {
        d = length % 128;
        length /= 128;
        if (length > 0) {
            d |= 0x80;
        }
        lenbuf.append(d);
    } while (length > 0);
}

} // namespace QMQTT
