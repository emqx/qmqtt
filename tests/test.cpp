#include <QtTest/QtTest>

class Test : public QObject
{
    Q_OBJECT
private slots:
    void failureTest();
};

void Test::failureTest()
{
    QVERIFY(1 == 2);
}

QTEST_MAIN(Test)
#include "test.moc"
