#include "messagetests.h"
#include <qmqtt_message.h>
#include <QTest>

MessageTests::MessageTests()
    : m_uut(NULL)
{
}

MessageTests::~MessageTests()
{
}

void MessageTests::init()
{
    m_uut.reset(new QMQTT::Message);
}

void MessageTests::cleanup()
{
    m_uut.reset();
}

void MessageTests::default_constructor()
{
    QCOMPARE(m_uut->id(), static_cast<quint16>(0));
    QCOMPARE(m_uut->qos(), static_cast<quint8>(0));
    QCOMPARE(m_uut->retain(), false);
    QCOMPARE(m_uut->dup(), false);
    QCOMPARE(m_uut->topic(), QString());
    QCOMPARE(m_uut->payload(), QByteArray());
}

void MessageTests::constructor_with_parameters_has_default_values()
{
    m_uut.reset(new QMQTT::Message(5, "topic", QString("payload").toLatin1()));
    QCOMPARE(m_uut->qos(), static_cast<quint8>(0));
    QCOMPARE(m_uut->retain(), false);
    QCOMPARE(m_uut->dup(), false);
}

void MessageTests::constructor_with_parameters()
{
    m_uut.reset(new QMQTT::Message(5, "topic", QString("payload").toLatin1(), 5, true, true));
    QCOMPARE(m_uut->id(), static_cast<quint16>(5));
    QCOMPARE(m_uut->qos(), static_cast<quint8>(5));
    QCOMPARE(m_uut->retain(), true);
    QCOMPARE(m_uut->dup(), true);
    QCOMPARE(m_uut->topic(), QString("topic"));
    QCOMPARE(m_uut->payload(), QByteArray("payload"));
}

void MessageTests::id_settable()
{
    m_uut->setId(5);
    QCOMPARE(m_uut->id(), static_cast<quint16>(5));
}

void MessageTests::qos_settable()
{
    m_uut->setQos(5);
    QCOMPARE(m_uut->qos(), static_cast<quint8>(5));
}

void MessageTests::retain_settable()
{
    m_uut->setRetain(true);
    QCOMPARE(m_uut->retain(), true);
}

void MessageTests::dup_settable()
{
    m_uut->setDup(true);
    QCOMPARE(m_uut->dup(), true);
}

void MessageTests::topic_settable()
{
    m_uut->setTopic("topic");
    QCOMPARE(m_uut->topic(), QString("topic"));
}

void MessageTests::payload_settable()
{
    m_uut->setPayload("payload");
    QCOMPARE(m_uut->payload(), QByteArray("payload"));
}
