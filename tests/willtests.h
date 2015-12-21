#ifndef WILL_TESTS_H
#define WILL_TESTS_H

#include <QObject>
#include <QScopedPointer>

namespace QMQTT
{
    class Will;
}

class WillTests : public QObject
{
    Q_OBJECT
public:
    explicit WillTests();
    virtual ~WillTests();

    QScopedPointer<QMQTT::Will> _uut;

private slots:
    void init();
    void cleanup();

    void qosDefaultsToZero_Test();
    void qosSettableToFive_Test();

    void retainDefaultsToFalse_Test();
    void retainSettableToTrue_Test();

    void topicSettableToAString_Test();
    void messageSettableToAString_Test();
};

#endif // WILL_TESTS_H
