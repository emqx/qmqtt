#ifndef QMQTT_WILL_H
#define QMQTT_WILL_H
#include <QObject>
#include <QString>

#include "qmqtt_global.h"

class QMQTTSHARED_EXPORT QMQTTWill : public QObject
{
    Q_OBJECT

public:
    QMQTTWill(QObject * parent = 0);
    //QMQTTWill(QString const &topic, QString const &msg, quint8 qos = 0, bool retain = false);
    ~QMQTTWill();

    quint8 qos();
    void setQos(quint8 qos);

    bool retain();
    void setRetain(bool retain);

    QString topic();
    void setTopic(const QString &topic);

    QString message();
    void setMessage(const QString &message);

private:
    quint8 _qos;
    bool _retain;
    QString _topic;
    QString _message;

};

#endif // QMQTT_WILL_H
