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
        : _argc(argc)
        , _argv(argv)
        , _exitCode(0)
    {
    }

    void exec(QObject* testObject)
    {
        int ec = QTest::qExec(testObject, _argc, _argv);
        if (_exitCode == 0)
        {
            _exitCode = ec;
        }
    }

    int _argc;
    char** _argv;
    int _exitCode;
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

    return runner._exitCode;
}
