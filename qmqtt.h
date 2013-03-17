#ifndef QMQTT_H
#define QMQTT_H

#include <QTimer>
#include <QObject>
#include <QTcpSocket>

#include "qmqtt_global.h"
#include "qmqtt_will.h"
#include "qmqtt_message.h"

#define MQTT_OK (0)
#define MQTT_ERR (-1)

/*
 * MQTT Version
 */
#define MQTT_PROTO_MAJOR 3
#define MQTT_PROTO_MINOR 1
#define MQTT_PROTOCOL_VERSION "MQTT/3.1"

/*
 * MQTT QOS
 */
#define MQTT_QOS0 0
#define MQTT_QOS1 1
#define MQTT_QOS2 2

/*
 * QMQTT Version
 */
#define QMQTT_VERSION "0.1.0"

class QMQTTSHARED_EXPORT QMQTT : public QObject
{
    Q_OBJECT

public:

    /*
     * MQTT ConnAck
     */
    enum MQTTConnAck
    {
        CONNACK_ACCEPT  = 0,
        CONNACK_PROTO_VER,
        CONNACK_INVALID_ID,
        CONNACK_SERVER,
        CONNACK_CREDENTIALS,
        CONNACK_AUTH
    };

    enum MQTTState
    {
        MQTT_STATE_INIT = 0,
        MQTT_STATE_CONNECTING,
        MQTT_STATE_CONNECTED,
        MQTT_STATE_DISCONNECTED
    };

    //QMQTT(QObject * parent = 0);
    QMQTT(const QString &host = "localhost", int port = 1883);
    ~QMQTT();

    /*
     * Property get and set
     */
    QString host();
    void setHost(const QString &host);
    int port();
    void setPort(int port);
    QString clientId();
    void setClientId(const QString &clientId);
    QString username();
    void setUsername(const QString &username);
    QString password();
    void setPassword(const QString &password);
    int keepalive();
    void setKeepAlive(int keepalive);
    QMQTTWill *will();
    void setWill(QMQTTWill *will);

    /*
     * MQTT Command
     */
    void connect();
    void publish(QMQTTMessage &message);
    void puback(int msgid);
    void pubrec(int msgid);
    void pubrel(int msgid);
    void pubcomp(int msgid);
    int subscribe(QString &topic, quint8 qos);
    void unsubscribe(QString &topic);
    void ping();
    void disconnect();

signals:
    void connectAcked(MQTTConnAck ack);
    void published(QMQTTMessage &message);
    void received(QMQTTMessage &message);
    void subscribed(quint16 msgid, quint8 qos);
    void unsubscribed(quint16 msgid);
    void pong();
    void disconnected();

private slots:
    void sendConnect();
    void sendAck(int type, quint16 msgid);
    void socketOpened();
    void socketRead();
    void socketError();
    void socketClosed();

private:
    void writeInt(QByteArray &buffer, int i);
    void writeString(QByteArray &buffer, const QString &string);
    void writeChar(QByteArray &buffer, char c);
    void writeLength(QByteArray &buffer, int len);
    void writeRawData(QByteArray &buffer, const QByteArray &data);

private:
    QString _host;
    int _port;
    int _msgid;

    QString _clientId;
    QString _username;
    QString _password;
    bool _cleansess;
    int _keepalive;
    QMQTTWill *_will;
    QTcpSocket *socket;
};

#endif // QMQTT_H
