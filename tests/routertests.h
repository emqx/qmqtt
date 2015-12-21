#ifndef ROUTER_TESTS_H
#define ROUTER_TESTS_H

#include <QObject>
#include <QScopedPointer>

namespace QMQTT
{
    class Router;
    class Client;
}

class RouterTests : public QObject
{
    Q_OBJECT
public:
    explicit RouterTests();
    virtual ~RouterTests();

    QScopedPointer<QMQTT::Client> _client;
    QScopedPointer<QMQTT::Router> _uut;

private slots:
    void init();
    void cleanup();

    void subscribe_Test();
};

#endif // ROUTER_TESTS_H
