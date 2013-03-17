#include "qmqtt_message.h"

QMQTTMessage::QMQTTMessage(QObject *parent)
    : QObject(parent)
{
    _id = 0;
    _qos = 0;
    _retain = false;
    _dup = false;
}

QMQTTMessage::~QMQTTMessage()
{
    //NOTHING TODO
}

quint16 QMQTTMessage::id()
{
    return _id;
}

void QMQTTMessage::setId(quint16 id)
{
    _id = id;
}

quint8 QMQTTMessage::qos()
{
    return _qos;
}

void QMQTTMessage::setQos(quint8 qos)
{
    _qos = qos;
}

bool QMQTTMessage::retain()
{
    return _retain;
}

void QMQTTMessage::setRetain(bool retain)
{
    _retain = retain;
}

bool QMQTTMessage::dup()
{
    return _dup;
}
void QMQTTMessage::setDup(bool dup)
{
    _dup =dup;
}

QString QMQTTMessage::topic()
{
    return _topic;
}

void QMQTTMessage::setTopic(const QString &topic)
{
    _topic = topic;
}

QByteArray QMQTTMessage::payload()
{
    return _payload;
}

void QMQTTMessage::setPayload(const QByteArray &payload)
{
    _payload = payload;
}

