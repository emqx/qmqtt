#include <qmqtt_message.h>
#include <QTest>
#include <QScopedPointer>

namespace QMQTT
{
    class Message;
}

class MessageTests : public QObject
{
    Q_OBJECT
public:
    explicit MessageTests();
    virtual ~MessageTests();

    QScopedPointer<QMQTT::Message> _uut;

private slots:
    void init();
    void cleanup();

    void defaultConstructor_Test();
    void constructorWithParametersHasDefaultValues_Test();
    void constructorWithParameters_Test();

    // todo: equality operator

    void idSettable_Test();
    void qosSettable_Test();
    void retainSettable_Test();
    void dupSettable_Test();
    void topicSettable_Test();
    void payloadSettable_Test();
};

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

void MessageTests::defaultConstructor_Test()
{
    QCOMPARE(_uut->id(), static_cast<quint16>(0));
    QCOMPARE(_uut->qos(), static_cast<quint8>(0));
    QCOMPARE(_uut->retain(), false);
    QCOMPARE(_uut->dup(), false);
    QCOMPARE(_uut->topic(), QString());
    QCOMPARE(_uut->payload(), QByteArray());
}

void MessageTests::constructorWithParametersHasDefaultValues_Test()
{
    _uut.reset(new QMQTT::Message(5, "topic", QString("payload").toUtf8()));
    QCOMPARE(_uut->qos(), static_cast<quint8>(0));
    QCOMPARE(_uut->retain(), false);
    QCOMPARE(_uut->dup(), false);
}

void MessageTests::constructorWithParameters_Test()
{
    _uut.reset(new QMQTT::Message(5, "topic", QString("payload").toUtf8(), 5, true, true));
    QCOMPARE(_uut->id(), static_cast<quint16>(5));
    QCOMPARE(_uut->qos(), static_cast<quint8>(5));
    QCOMPARE(_uut->retain(), true);
    QCOMPARE(_uut->dup(), true);
    QCOMPARE(_uut->topic(), QString("topic"));
    QCOMPARE(_uut->payload(), QByteArray("payload"));
}

void MessageTests::idSettable_Test()
{
    _uut->setId(5);
    QCOMPARE(_uut->id(), static_cast<quint16>(5));
}

void MessageTests::qosSettable_Test()
{
    _uut->setQos(5);
    QCOMPARE(_uut->qos(), static_cast<quint8>(5));
}

void MessageTests::retainSettable_Test()
{
    _uut->setRetain(true);
    QCOMPARE(_uut->retain(), true);
}

void MessageTests::dupSettable_Test()
{
    _uut->setDup(true);
    QCOMPARE(_uut->dup(), true);
}

void MessageTests::topicSettable_Test()
{
    _uut->setTopic("topic");
    QCOMPARE(_uut->topic(), QString("topic"));
}

void MessageTests::payloadSettable_Test()
{
    _uut->setPayload("payload");
    QCOMPARE(_uut->payload(), QByteArray("payload"));
}

QTEST_MAIN(MessageTests);
#include "messagetests.moc"
