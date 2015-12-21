#include "routertests.h"
#include <qmqtt_router.h>
#include <qmqtt_client.h>
#include <qmqtt_routesubscription.h>
#include <QTest>
#include <QSignalSpy>

RouterTests::RouterTests()
    : _uut(NULL)
{
}

RouterTests::~RouterTests()
{
}

void RouterTests::init()
{
    _client.reset(new QMQTT::Client);
    _uut.reset(new QMQTT::Router(_client.data()));
}

void RouterTests::cleanup()
{
    _uut.reset();
    _client.reset();
}

void RouterTests::subscribe_Test()
{
    QMQTT::RouteSubscription* subscription = _uut->subscribe("/route");
    QVERIFY(NULL != subscription);
    QCOMPARE(subscription->route(), QString("/route"));
}

// todo: need to figure out how to test subscribe a little better
