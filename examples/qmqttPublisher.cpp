#include <qmqtt_client.h>
#include <iostream>
#include <QObject>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QTimer>

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
    int id = qrand();
    QCoreApplication a(argc, argv);
    QCommandLineParser parser;
    QCommandLineOption hostOption("host",
            QCoreApplication::translate("host", "The MQTT host to connect, localhost used if not defined."),
            "host", "localhost");
    QCommandLineOption qosOption("qos",
            QCoreApplication::translate("qos", "Quality of Service level, 0 used if not defined."),
            "qos", "0");
    QCommandLineOption portOption("port",
            QCoreApplication::translate("port", "The MQTT port to connect, 1883 used if not defined."),
            "port", "1883");

    parser.addPositionalArgument("topic", QCoreApplication::translate("main", "Topic to subscribe"));
    parser.addOption(hostOption);
    parser.addOption(qosOption);
    parser.addOption(portOption);
    parser.process(a);
    QStringList args = parser.positionalArguments();

    if (args.size() < 1) {
        parser.showHelp(0);
        return 0;
    }

    QString host = parser.value("host");
    quint32 port = parser.value("port").toUInt();
    QString qos = parser.value("qos");
    QString topic = args.at(0);

    cout << "using: host[" << host.toStdString() << "] port[" << port << "] qos[" << qos.toStdString() << "]" << endl;
    
    QString message = QString("message from %1").arg(id);
    Message msg(id, topic, message.toUtf8());
    MyClient c(host, port);
    c.setMessage(msg);
    Logger p;
    QTimer t;
    cout << "publish on topic[" << topic.toStdString() << "]" << " with message[" << message.toStdString() << "]" << endl;

    t.setInterval(1000);

    QObject::connect(&c, &MyClient::connected, &p, &Logger::showConnected);
    QObject::connect(&c, &MyClient::published, &p, &Logger::showPublished);
    QObject::connect(&c, &MyClient::disconnected, &p, &Logger::showDisconnected);
    QObject::connect(&c, &MyClient::error, &p, &Logger::showError);
    QObject::connect(&t, &QTimer::timeout, &c, &MyClient::sendMessage);

    QObject::connect(&c, SIGNAL(connected()), &t, SLOT(start()));
    QObject::connect(&c, SIGNAL(disconnected()), &t , SLOT(stop()));
    QObject::connect(&a, SIGNAL(aboutToQuit()), &c, SLOT(disconnect()));

    c.connect();

    return a.exec();
}
