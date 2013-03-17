
#include "qmqtt_will.h"

QMQTTWill::QMQTTWill(QObject *parent) :
    QObject(parent)
{
    _qos = 0;
    _retain = false;
}

/*
QMQTTWill::QMQTTWill(QString const &topic, QString const &message, int qos, bool retain) :
    _topic(topic),
    _message(message),
    _qos(qos),
    _retain(retain)
{
}
*/

QMQTTWill::~QMQTTWill()
{
    //NOTHING TODO;
}

quint8 QMQTTWill::qos()
{
    return _qos;
}

void QMQTTWill::setQos(quint8 qos)
{
    _qos = qos;
}

bool QMQTTWill::retain()
{
    return _retain;
}

void QMQTTWill::setRetain(bool retain)
{
    _retain = retain;
}

QString QMQTTWill::topic()
{
    return _topic;
}

void QMQTTWill::setTopic(const QString &topic)
{
    _topic = topic;
}

QString QMQTTWill::message()
{
    return _message;
}

void QMQTTWill::setMessage(const QString &message)
{
    _message = message;
}

