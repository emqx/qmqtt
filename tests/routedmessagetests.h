#ifndef ROUTED_MESSAGE_TESTS_H
#define ROUTED_MESSAGE_TESTS_H

#include <QObject>
#include <QScopedPointer>

namespace QMQTT
{
    class RoutedMessage;
}

class RoutedMessageTests : public QObject
{
    Q_OBJECT
public:
    explicit RoutedMessageTests();
    virtual ~RoutedMessageTests();

    QScopedPointer<QMQTT::RoutedMessage> _uut;

private slots:
    void init();
    void cleanup();

    void constructor_Test();
};

#endif // ROUTED_MESSAGE_TESTS_H
