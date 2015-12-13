#include <QtTest>

#include "sampletest.h"
#include "sampletest2.h"

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

    SampleTest sampleTest;
    runner.exec(&sampleTest);

    SampleTest2 sampleTest2;
    runner.exec(&sampleTest2);

    return runner.ExitCode;
}
