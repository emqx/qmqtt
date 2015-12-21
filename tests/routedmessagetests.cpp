#include "routedmessagetests.h"
#include <qmqtt_routedmessage.h>
#include <QTest>

RoutedMessageTests::RoutedMessageTests()
    : _uut(NULL)
{
}

RoutedMessageTests::~RoutedMessageTests()
{
}

void RoutedMessageTests::init()
{
    _uut.reset(new QMQTT::RoutedMessage(QMQTT::Message()));
}

void RoutedMessageTests::cleanup()
{
    _uut.reset();
}

void RoutedMessageTests::constructor_Test()
{
    QCOMPARE(_uut->message(), QMQTT::Message());
    QHash<QString, QString> emptyHash;
    QCOMPARE(_uut->parameters(), emptyHash);
}
