#include "willtests.h"
#include <qmqtt_will.h>
#include <QTest>

WillTests::WillTests()
    : m_uut(NULL)
{
}

WillTests::~WillTests()
{
}

void WillTests::init()
{
    m_uut.reset(new QMQTT::Will("", ""));
}

void WillTests::cleanup()
{
    m_uut.reset();
}

void WillTests::qos_defaults_to_zero()
{
    QCOMPARE(m_uut->qos(), static_cast<quint8>(0));
}

void WillTests::qos_settable_to_five()
{
    m_uut->setQos(5);
    QCOMPARE(m_uut->qos(), static_cast<quint8>(5));
}

void WillTests::retain_defaults_to_false()
{
    QCOMPARE(m_uut->retain(), false);
}

void WillTests::retain_settable_to_true()
{
    m_uut->setRetain(true);
    QCOMPARE(m_uut->retain(), true);
}

void WillTests::topic_settable_to_a_string()
{
    m_uut->setTopic(QString("topic"));
    QCOMPARE(m_uut->topic(), QString("topic"));
}

void WillTests::message_settable_to_a_string()
{
    m_uut->setMessage(QString("message"));
    QCOMPARE(m_uut->message(), QString("message"));
}
