#ifndef MESSAGE_TESTS_H
#define MESSAGE_TESTS_H

#include <QObject>
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

#endif // MESSAGE_TESTS_H
