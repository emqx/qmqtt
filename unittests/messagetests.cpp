#include "messagetests.h"
#include <qmqtt_message.h>
#include <QTest>

MessageTests::MessageTests()
    : _uut(NULL)
{
}

MessageTests::~MessageTests()
{
}

void MessageTests::init()
{
    _uut.reset(new QMQTT::Message);
}

void MessageTests::cleanup()
{
    _uut.reset();
}

void MessageTests::default_constructor()
{
    QCOMPARE(_uut->id(), static_cast<quint16>(0));
    QCOMPARE(_uut->qos(), static_cast<quint8>(0));
    QCOMPARE(_uut->retain(), false);
    QCOMPARE(_uut->dup(), false);
    QCOMPARE(_uut->topic(), QString());
    QCOMPARE(_uut->payload(), QByteArray());
}

void MessageTests::constructor_with_parameters_has_default_values()
{
    _uut.reset(new QMQTT::Message(5, "topic", QString("payload").toLatin1()));
    QCOMPARE(_uut->qos(), static_cast<quint8>(0));
    QCOMPARE(_uut->retain(), false);
    QCOMPARE(_uut->dup(), false);
}

void MessageTests::constructor_with_parameters()
{
    _uut.reset(new QMQTT::Message(5, "topic", QString("payload").toLatin1(), 5, true, true));
    QCOMPARE(_uut->id(), static_cast<quint16>(5));
    QCOMPARE(_uut->qos(), static_cast<quint8>(5));
    QCOMPARE(_uut->retain(), true);
    QCOMPARE(_uut->dup(), true);
    QCOMPARE(_uut->topic(), QString("topic"));
    QCOMPARE(_uut->payload(), QByteArray("payload"));
}

void MessageTests::id_settable()
{
    _uut->setId(5);
    QCOMPARE(_uut->id(), static_cast<quint16>(5));
}

void MessageTests::qos_settable()
{
    _uut->setQos(5);
    QCOMPARE(_uut->qos(), static_cast<quint8>(5));
}

void MessageTests::retain_settable()
{
    _uut->setRetain(true);
    QCOMPARE(_uut->retain(), true);
}

void MessageTests::dup_settable()
{
    _uut->setDup(true);
    QCOMPARE(_uut->dup(), true);
}

void MessageTests::topic_settable()
{
    _uut->setTopic("topic");
    QCOMPARE(_uut->topic(), QString("topic"));
}

void MessageTests::payload_settable()
{
    _uut->setPayload("payload");
    QCOMPARE(_uut->payload(), QByteArray("payload"));
}
