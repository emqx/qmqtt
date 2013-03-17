
#include "qmqtt.h"
#include "qmqtt_packet.h"

#include <QByteArray>
#include <QDataStream>

//TODO: design signals and slots

QMQTT::QMQTT(const QString &host, int port) :
    _host(host),
    _port(port)
{
    _msgid = 1;
    socket = new QTcpSocket(this);
    //TODO: connect signal and slots
    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(socketRead()));
    QObject::connect(socket, SIGNAL(connected()), this, SLOT(sockedOpened()));
    QObject::connect(socket, SIGNAL(disconnect()), this, SLOT(socketClosed()));
    //connect(socket, SIGNAL(error(QAbstractSocket::SocketError socketError)), this, SLOT());
    //void	stateChanged(QAbstractSocket::SocketState socketState)
}

QMQTT::~QMQTT() {
    //TODO: socket.close()
}

/*
 * Property get and set
 */
QString QMQTT::host()
{
    return _host;
}

void QMQTT::setHost(const QString &host)
{
    _host = host;
}

int QMQTT::port()
{
    return _port;
}

void QMQTT::setPort(int port)
{
    _port = port;
}

QString QMQTT::clientId()
{
    return _clientId;
}

void QMQTT::setClientId(const QString &clientId)
{
    _clientId = clientId;
}

QString QMQTT::username()
{
    return _username;
}

void QMQTT::setUsername(const QString &username)
{
    _username = username;
}

QString QMQTT::password()
{
    return _password;
}

void QMQTT::setPassword(const QString &password)
{
    _password = password;
}

int QMQTT::keepalive()
{
    return _keepalive;
}

void QMQTT::setKeepAlive(int keepalive)
{
    _keepalive = keepalive;
}

QMQTTWill *QMQTT::will()
{
    return _will;
}

void QMQTT::setWill(QMQTTWill *will)
{
    _will = will;
}

void QMQTT::connect()
{
    //TODO: socket connect to host;
    socket->connectToHost(_host, _port);
    //wait for connected and send connect!
    if (socket->waitForConnected(5000)) {
        qDebug("Connected!");
        sendConnect();
    }
    //TODO: timer
}

void QMQTT::sendConnect()
{
    QByteArray buffer;
    QDataStream out(socket);
    QString magic(PROTOCOL_MAGIC);
    quint8 header = CONNECT;
    quint8 flags = 0;
    int len = 0;

    //header
    header = SETQOS(header, MQTT_QOS1);

    //flags
    flags = FLAG_CLEANSESS(flags, _cleansess ? 1 : 0 );
    flags = FLAG_WILL(flags, _will ? 1 : 0);

    if (_will) {
        flags = FLAG_WILLQOS(flags, _will->qos());
        flags = FLAG_WILLRETAIN(flags, _will->retain() ? 1 : 0);
    }
    if (!_username.isEmpty()) flags = FLAG_USERNAME(flags, 1);
    if (!_password.isEmpty()) flags = FLAG_PASSWD(flags, 1);

    //length
    if(!_clientId.isEmpty()) {
        len = 12 + 2 + _clientId.size();
    }
    if(_will != NULL) {
        len += 2 + _will->topic().size();
        len += 2 + _will->message().size();
    }
    if(!_username.isEmpty()) {
        len += 2 + _username.size();
    }
    if(!_password.isEmpty()) {
        len += 2 + _password.size();
    }

    writeChar(buffer, header);
    writeLength(buffer, len);
    writeString(buffer, magic);
    writeChar(buffer, MQTT_PROTO_MAJOR);
    writeChar(buffer, flags);
    writeInt(buffer, _keepalive);
    writeString(buffer, _clientId);

    if(_will != NULL) {
        writeString(buffer, _will->topic());
        writeString(buffer, _will->message());
    }
    if (!_username.isEmpty()) {
        writeString(buffer, _username);
    }
    if (!_password.isEmpty()) {
        writeString(buffer, _password);
    }
    out.writeRawData(buffer.data(), buffer.size());
}

void QMQTT::publish(QMQTTMessage &msg)
{
    QByteArray buffer;
    QDataStream out(socket);

    int len = 0;
    quint8 header = PUBLISH;
    header = SETRETAIN(header, msg.retain() ? 1 : 0);
    header = SETQOS(header, msg.qos());
    header = SETDUP(header, msg.dup() ? 1 : 0);
    buffer.append(header);

    len += 2 + msg.topic().size();
    if(msg.qos() > MQTT_QOS0) len += 2; //msgid
    if(!msg.payload().isEmpty()) len += msg.payload().size();
    writeLength(buffer, len);
    writeString(buffer, msg.topic());
    if(msg.qos() > MQTT_QOS0) {
        //TODO: msg id
        writeInt(buffer, msg.id());
    }
    if(!msg.payload().isEmpty()) {
        writeRawData(buffer, msg.payload());
    }
    out.writeRawData(buffer.data(), buffer.size());
}

void QMQTT::puback(int msgid)
{
    sendAck(PUBACK, msgid);
}

void QMQTT::pubrec(int msgid)
{
    sendAck(PUBREC, msgid);
}

void QMQTT::pubrel(int msgid)
{
    sendAck(PUBREL, msgid);
}

void QMQTT::pubcomp(int msgid)
{
    sendAck(PUBCOMP, msgid);
}

void QMQTT::sendAck(int type, quint16 msgid)
{
    QDataStream out(socket);
    char buffer[4] = {type, 2, MSB(msgid), LSB(msgid)};
    out.writeRawData(buffer, 4);
}

int QMQTT::subscribe(QString &topic, quint8 qos)
{
    QByteArray buffer;
    QDataStream out(socket);
    int msgid = _msgid++;
    buffer.append(SETQOS(SUBSCRIBE, MQTT_QOS1));
    writeLength(buffer, 2+2+topic.size()+1);//msgi + topic + qos
    writeInt(buffer, msgid);
    writeString(buffer, topic);
    writeChar(buffer, qos);

    out.writeRawData(buffer.data(), buffer.size());

    emit subscribed(msgid, qos);

    return msgid;
}

void QMQTT::unsubscribe(QString &topic) {
    QByteArray buffer;
    QDataStream out(socket);
    buffer.append(SETQOS(UNSUBSCRIBE, MQTT_QOS1));
    writeLength(buffer, 2+2+topic.size());//msgi + topic
    writeInt(buffer, _msgid++);
    writeString(buffer, topic);

    out.writeRawData(buffer.data(), buffer.size());
}

void QMQTT::ping()
{
    char buffer[2] = {PINGREQ, 0};
    QDataStream stream(socket);
    //TODO: how to handle error?
    stream.writeRawData(buffer, 2);
}

void QMQTT::disconnect()
{
    char buffer[2] = {DISCONNECT, 0};
    QDataStream stream(socket);
    //TODO: how to handle error?
    stream.writeRawData(buffer, 2);
}

/*
 *--------------------------------------
 * Slots
 *---------------------------------------
 */

void QMQTT::socketOpened()
{
    qDebug("session opened...");
}

void QMQTT::socketRead()
{
    qDebug("socket read...");
}

void QMQTT::socketError()
{
    qDebug("socket error...");
}

void QMQTT::socketClosed()
{
    qDebug("socket closed...");
}

/*
 * Priavte Method
 */
void QMQTT::writeLength(QByteArray &buffer, int length)
{
    char d;
    do {
            d = length % 128;
            length /= 128;
            if (length > 0)
                d |= 0x80;
            buffer.append(d);
    } while (length > 0);
}

void QMQTT::writeInt(QByteArray &buffer, int i)
{
    buffer.append(MSB(i));
    buffer.append(LSB(i));
}

void QMQTT::writeString(QByteArray &buffer, const QString &string)
{
    writeInt(buffer, string.size());
    buffer.append(string);
}

void QMQTT::writeChar(QByteArray &buffer, char c)
{
    buffer.append(c);
}

void QMQTT::writeRawData(QByteArray &buffer, const QByteArray &data)
{
    buffer.append(data);
}
