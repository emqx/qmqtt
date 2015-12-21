#include "willtests.h"
#include <qmqtt_will.h>
#include <QTest>

WillTests::WillTests()
    : _uut(NULL)
{
}

WillTests::~WillTests()
{
}

void WillTests::init()
{
    _uut.reset(new QMQTT::Will("", ""));
}

void WillTests::cleanup()
{
    _uut.reset();
}

void WillTests::qosDefaultsToZero_Test()
{
    QCOMPARE(_uut->qos(), static_cast<quint8>(0));
}

void WillTests::qosSettableToFive_Test()
{
    _uut->setQos(5);
    QCOMPARE(_uut->qos(), static_cast<quint8>(5));
}

void WillTests::retainDefaultsToFalse_Test()
{
    QCOMPARE(_uut->retain(), false);
}

void WillTests::retainSettableToTrue_Test()
{
    _uut->setRetain(true);
    QCOMPARE(_uut->retain(), true);
}

void WillTests::topicSettableToAString_Test()
{
    _uut->setTopic(QString("topic"));
    QCOMPARE(_uut->topic(), QString("topic"));
}

void WillTests::messageSettableToAString_Test()
{
    _uut->setMessage(QString("message"));
    QCOMPARE(_uut->message(), QString("message"));
}
