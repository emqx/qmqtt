#ifndef SAMPLE_TEST_H
#define SAMPLE_TEST_H

#include <QTest>

class SampleTest : public QObject
{
    Q_OBJECT
private slots:
    void some_test()
    {
        QCOMPARE(1, 1);
    }

    void some_other_test()
    {
        QCOMPARE(3, 3);
    }

};

#endif // SAMPLE_TEST_H
