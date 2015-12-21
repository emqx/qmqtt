#ifndef ROUTE_SUBSCRIPTION_TEST_H
#define ROUTE_SUBSCRIPTION_TEST_H

#include <QObject>
#include <QScopedPointer>

namespace QMQTT
{
    class RouteSubscription;
}

class RouteSubscriptionTests : public QObject
{
    Q_OBJECT
public:
    explicit RouteSubscriptionTests();
    virtual ~RouteSubscriptionTests();

    QScopedPointer<QMQTT::RouteSubscription> _uut;

private slots:
    void init();
    void cleanup();

    // todo: can't really test directly here, only Route class can insantiate for now
};

#endif // ROUTE_SUBSCRIPTION_TEST_H
