#include <QString>
#include <qmqtt_client.h>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QObject>

using QMQTT::Client;
using QMQTT::Will;
using QMQTT::Message;


#include <iostream>
using namespace std;


class Subscriber : public QObject {
    public:
        Subscriber() : QObject(0) {
        }
        ~Subscriber() {
        }
    public slots:
        void showMqttData(const QMQTT::Message &message) {
            QString data(message.payload());
            cout << "received data: " << data.toStdString() << endl;
        }
        void showSubscribed() {
            cout << "subscribed" << endl;
        }
        void showConnected() {
            cout << "connected!" << endl;
        }
        void showDisConnected() {
            cout << "disconnected!" << endl;
        }
};

int main(int argc, char ** argv)
{
    QString host = "127.0.0.1";
    QString clientId = "QtClient";
    QString topic = "farm/gw/+/data";
    quint32 port = 1883;
    quint8 qos = 0;

    QCoreApplication a(argc, argv);
   // QCommandLineParser parser;
   // parser.addPositionalArgument("topic", QCoreApplication::translate("main", "Topic to subscribe"));
   // parser.addPositionalArgument("qos", QCoreApplication::translate("main", "QoS"));

   // QCommandLineOption idOption(QStringList() << "i" << "Client Id");
   // parser.addOption(idOption);
   // parser.parse(a);
   // QStringList args = parser.positionalArguments();
   // topic = args.at(0);
   // qos = args.at(1).toInt();

    Subscriber s;
    Client c(host, port);
    QObject::connect(&c, &Client::connected, [&] () {
        s.showConnected();
        c.subscribe(topic, qos);
    });
    QObject::connect(&c, &Client::disconnected, &s, &Subscriber::showDisConnected);
    QObject::connect(&c, &Client::subscribed, &s, &Subscriber::showSubscribed);
    QObject::connect(&c, &Client::received, &s, &Subscriber::showMqttData);
    
    c.connect();

    a.exec();

    c.unsubscribed(topic);
    c.disconnect();
}
