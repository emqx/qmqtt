#include "routesubscriptiontests.h"
#include <qmqtt_routesubscription.h>
#include <QTest>

RouteSubscriptionTests::RouteSubscriptionTests()
    : _uut(NULL)
{
}

RouteSubscriptionTests::~RouteSubscriptionTests()
{
}

void RouteSubscriptionTests::init()
{
    _uut.reset();
}

void RouteSubscriptionTests::cleanup()
{
    _uut.reset();
}
