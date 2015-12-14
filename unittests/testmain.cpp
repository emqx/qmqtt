#include "willtests.h"
#include "messagetests.h"
#include <QtTest>

class TestRunner
{
public:
    TestRunner(int argc = 0, char** argv = NULL)
        : Argc(argc)
        , Argv(argv)
        , ExitCode(0)
    {
    }

    void exec(QObject* testObject)
    {
        int ec = QTest::qExec(testObject, Argc, Argv);
        if (ExitCode == 0)
        {
            ExitCode = ec;
        }
    }

    int Argc;
    char** Argv;
    int ExitCode;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TestRunner runner(argc, argv);

    WillTests willTests;
    runner.exec(&willTests);

    MessageTests messageTests;
    runner.exec(&messageTests);

    return runner.ExitCode;
}
