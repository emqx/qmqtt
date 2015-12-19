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

void WillTests::qos_defaults_to_zero()
{
    QCOMPARE(_uut->qos(), static_cast<quint8>(0));
}

void WillTests::qos_settable_to_five()
{
    _uut->setQos(5);
    QCOMPARE(_uut->qos(), static_cast<quint8>(5));
}

void WillTests::retain_defaults_to_false()
{
    QCOMPARE(_uut->retain(), false);
}

void WillTests::retain_settable_to_true()
{
    _uut->setRetain(true);
    QCOMPARE(_uut->retain(), true);
}

void WillTests::topic_settable_to_a_string()
{
    _uut->setTopic(QString("topic"));
    QCOMPARE(_uut->topic(), QString("topic"));
}

void WillTests::message_settable_to_a_string()
{
    _uut->setMessage(QString("message"));
    QCOMPARE(_uut->message(), QString("message"));
}
