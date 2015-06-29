#include <qmqtt_client.h>
#include <iostream>
#include <QObject>
#include <QThread>
#include <QCoreApplication>

using QMQTT::Client;
using QMQTT::Message;
using QMQTT::Will;

using namespace std;

class Publisher : public QObject {
    public:
        Publisher(QObject * parent = 0) : QObject(0) {
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
    private:
        QString topic, message, willTopic;
        Will * lwt;
    
};

int main(int argc, char ** argv)
{
    int id = 140;
    QCoreApplication a(argc, argv);
    QString topic = QString("farm/gw/%1/data").arg(id);
    QString message = QString("message from %1").arg(id);
    Message msg(id, topic, message.toUtf8());
    Client c;
    Publisher p;

    QObject::connect(&c, &Client::connected, [&] () {
        p.showConnected();
        c.publish(msg);
    });
    QObject::connect(&c, &Client::published, &p, &Publisher::showPublished);
    QObject::connect(&c, &Client::disconnected, &p, &Publisher::showDisconnected);
    QObject::connect(&c, &Client::error, &p, &Publisher::showError);

    c.connect();

    return a.exec();
}
