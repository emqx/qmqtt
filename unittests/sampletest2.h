#ifndef SAMPLE_TEST_2_H
#define SAMPLE_TEST_2_H

#include <QTest>

class SampleTest2 : public QObject
{
    Q_OBJECT
private slots:
    void some_test()
    {
        QCOMPARE(2, 2);
    }
};

#endif // SAMPLE_TEST_2_H
