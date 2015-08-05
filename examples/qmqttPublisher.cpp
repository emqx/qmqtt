#include <qmqtt_client.h>
#include <iostream>
#include <QObject>
#include <QThread>
#include <QCoreApplication>
#include <QCommandLineParser>

using QMQTT::Client;
using QMQTT::Message;
using QMQTT::Will;

using namespace std;

class Logger : public QObject {
    public:
        Logger(QObject * parent = 0) : QObject(0) {
        }
    public slots:
        void showError(QAbstractSocket::SocketError error) {
            cout << "error: " << error << endl;
        }
        void showDisconnected() {
            cout << "disconnected" << endl;
        }
        void showPublished () {
            cout << "published" << endl;
        }
        void showConnected() {
            cout << "connected" << endl;
        }
        void publish() {
        }
    private:
        QString topic, message, willTopic;
        Will * lwt;
    
};

class MyClient : public Client {
    public:
    MyClient(const QString & host, quint32 port) : Client(host, port) {
    }
    void sendMessage() {
        publish(toPublish);
    }

    void setMessage(const Message & msg) {
        toPublish = msg;
    }
    private:
    Message toPublish;
};

int main(int argc, char ** argv)
{
    int id = 140;
    QCoreApplication a(argc, argv);
    QCommandLineParser parser;
    parser.addPositionalArgument("topic", QCoreApplication::translate("main", "Topic to subscribe"));

    QCommandLineOption hostOption("host",
            QCoreApplication::translate("host", "The MQTT host to connect, localhost used if not defined."),
            "hostOption", "localhost");
    QCommandLineOption qosOption("qos",
            QCoreApplication::translate("qos", "Quality of Service level, 0 used if not defined."),
            "qosOption", "0");
    QCommandLineOption portOption("port",
            QCoreApplication::translate("port", "The MQTT port to connect, 1883 used if not defined."),
            "portOption", "1883");
    parser.process(a);
    QStringList args = parser.positionalArguments();
    if (args.size() < 2) {
        parser.showHelp(0);
        return 0;
    }
    QString host = parser.value("host");
    quint32 port = parser.value("port").toUInt();
    QString qos = parser.value("qos");
    QString topic = args.at(0);
    
    QString message = QString("message from %1").arg(id);
    Message msg(id, topic, message.toUtf8());
    MyClient c(host, port);
    c.setMessage(msg);
    Logger p;

    QObject::connect(&c, &MyClient::connected, &p , &Logger::showConnected);
    QObject::connect(&c, &MyClient::connected, &c , &MyClient::sendMessage);
    QObject::connect(&c, &MyClient::published, &p, &Logger::showPublished);
    QObject::connect(&c, &MyClient::disconnected, &p, &Logger::showDisconnected);
    QObject::connect(&c, &MyClient::error, &p, &Logger::showError);

    return a.exec();
}
