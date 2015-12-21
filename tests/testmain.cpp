#include "willtests.h"
#include "messagetests.h"
#include "frametests.h"
#include "networktests.h"
#include "routedmessagetests.h"
#include "routertests.h"
#include "routesubscriptiontests.h"
#include "clienttests.h"
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

//    WillTests willTests;
//    runner.exec(&willTests);

//    MessageTests messageTests;
//    runner.exec(&messageTests);

//    FrameTests frameTests;
//    runner.exec(&frameTests);

//    NetworkTests networkTests;
//    runner.exec(&networkTests);

//    RoutedMessageTests routedMessageTests;
//    runner.exec(&routedMessageTests);

//    RouterTests routerTests;
//    runner.exec(&routerTests);

//    RouteSubscriptionTests routeSubscriptionTests;
//    runner.exec(&routeSubscriptionTests);

    ClientTests clientTests;
    runner.exec(&clientTests);

    return runner.ExitCode;
}
