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

    QScopedPointer<QMQTT::Message> m_uut;

private slots:
    void init();
    void cleanup();

    void default_constructor();
    void constructor_with_parameters_has_default_values();
    void constructor_with_parameters();

    void id_settable();
    void qos_settable();
    void retain_settable();
    void dup_settable();
    void topic_settable();
    void payload_settable();
};

#endif // MESSAGE_TESTS_Hs
