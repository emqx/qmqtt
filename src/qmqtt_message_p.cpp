#include "qmqtt_message_p.h"

QMQTT::MessagePrivate::MessagePrivate()
    : _id(0)
    , _qos(0)
    , _retain(false)
    , _dup(false)
{
}

QMQTT::MessagePrivate::~MessagePrivate()
{
}

void QMQTT::MessagePrivate::init(quint16 id, const QString& topic,
                                 const QByteArray& payload, quint8 qos,
                                 bool retain, bool dup)
{
    _id = id;
    _topic = topic;
    _payload = payload;
    _qos = qos;
    _retain = retain;
    _dup = dup;
}

QMQTT::MessagePrivate::MessagePrivate(const MessagePrivate& other)
{
    _id = other._id;
    _topic = other._topic;
    _payload = other._payload;
    _qos = other._qos;
    _retain = other._retain;
    _dup = other._dup;
}

QMQTT::MessagePrivate& QMQTT::MessagePrivate::operator=(const MessagePrivate& other)
{
    if(this != &other)
    {
        _id = other._id;
        _topic = other._topic;
        _payload = other._payload;
        _qos = other._qos;
        _retain = other._retain;
        _dup = other._dup;
    }
    return *this;
}

bool QMQTT::MessagePrivate::operator==(const MessagePrivate& other) const
{
  return _id == other._id
      && _topic == other._topic
      && _payload == other._payload
      && _qos == other._qos
      && _retain == other._retain
      && _dup == other._dup;
}

quint16 QMQTT::MessagePrivate::id() const
{
    return _id;
}

void QMQTT::MessagePrivate::setId(const quint16 id)
{
    _id = id;
}

quint8 QMQTT::MessagePrivate::qos() const
{
    return _qos;
}

void QMQTT::MessagePrivate::setQos(const quint8 qos)
{
    _qos = qos;
}

bool QMQTT::MessagePrivate::retain() const
{
    return _retain;
}

void QMQTT::MessagePrivate::setRetain(const bool retain)
{
    _retain = retain;
}

bool QMQTT::MessagePrivate::dup() const
{
    return _dup;
}
void QMQTT::MessagePrivate::setDup(const bool dup)
{
    _dup =dup;
}

QString QMQTT::MessagePrivate::topic() const
{
    return _topic;
}

void QMQTT::MessagePrivate::setTopic(const QString& topic)
{
    _topic = topic;
}

QByteArray QMQTT::MessagePrivate::payload() const
{
    return _payload;
}

void QMQTT::MessagePrivate::setPayload(const QByteArray& payload)
{
    _payload = payload;
}
