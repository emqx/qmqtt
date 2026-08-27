#ifndef CUSTOM_PRINTER_H
#define CUSTOM_PRINTER_H

#include <QString>
#include <QTextStream>
#include <gtest/gtest.h>

// Qt 6 removed the global QTextStream manipulators, which had moved into the
// Qt namespace in 5.14.  Pull endl back into scope there and only there: Qt 5
// still declares the global one, and having both makes the name ambiguous.
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
using Qt::endl;
#endif

class CustomPrinter : public testing::EmptyTestEventListener
{
public:
    CustomPrinter();

    QTextStream qout;
    QTextStream qerr;

    void PrintTestName(const char* test_case, const char* test)
    {
        qout << test_case << "." << test;
    }

    virtual void OnTestProgramStart(const testing::UnitTest& /*unit_test*/) {}
    virtual void OnTestIterationStart(const testing::UnitTest& unit_test, int iteration);
    virtual void OnEnvironmentsSetUpStart(const testing::UnitTest& unit_test);
    virtual void OnEnvironmentsSetUpEnd(const testing::UnitTest& /*unit_test*/) {}
    virtual void OnTestCaseStart(const testing::TestCase& test_case);
    virtual void OnTestStart(const testing::TestInfo& test_info);
    //virtual void OnTestPartResult(const testing::TestPartResult& result);
    virtual void OnTestEnd(const testing::TestInfo& test_info);
    virtual void OnTestCaseEnd(const testing::TestCase& test_case);
    virtual void OnEnvironmentsTearDownStart(const testing::UnitTest& unit_test);
    virtual void OnEnvironmentsTearDownEnd(const testing::UnitTest& /*unit_test*/) {}
    virtual void OnTestIterationEnd(const testing::UnitTest& unit_test, int iteration);
    virtual void OnTestProgramEnd(const testing::UnitTest& /*unit_test*/) {}

private:
    static void PrintFailedTests(const testing::UnitTest& unit_test);
};

#endif // CUSTOM_PRINTER_H
