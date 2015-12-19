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

    void qos_defaults_to_zero();
    void qos_settable_to_five();

    void retain_defaults_to_false();
    void retain_settable_to_true();

    void topic_settable_to_a_string();
    void message_settable_to_a_string();

    //        void setMessage(const QString &message);

//        QString topic() const;
//        void setTopic(const QString & topic);

//        QString message() const;
//        void setMessage(const QString &message);

};

#endif // WILL_TESTS_H
