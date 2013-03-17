#ifndef QMQTT_MESSAGE_H
#define QMQTT_MESSAGE_H

#include "qmqtt_global.h"

#include <QObject>
#include <QString>
#include <QByteArray>

class QMQTTSHARED_EXPORT QMQTTMessage : public QObject
{
    Q_OBJECT

public:
    QMQTTMessage(QObject *parent = 0);
    ~QMQTTMessage();

    quint16 id();
    void setId(quint16 id);

    quint8 qos();
    void setQos(quint8 qos);

    bool retain();
    void setRetain(bool retain);

    bool dup();
    void setDup(bool dup);

    QString topic();
    void setTopic(const QString &topic);

    QByteArray payload();
    void setPayload(const QByteArray &payload);

private:
    quint16 _id;
    quint8 _qos;
    bool _retain;
    bool _dup;
    QString _topic;
    QByteArray _payload;
};

#endif // QMQTT_MESSAGE_H
