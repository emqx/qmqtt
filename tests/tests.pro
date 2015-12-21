QT += widgets testlib network
QT -= gui
TARGET = qmqtt_tests
DEFINES += QMQTT_LIBRARY_TESTS
CONFIG += testcase

SOURCES += \
    testmain.cpp \
    willtests.cpp \
    messagetests.cpp \
    frametests.cpp \
    networktests.cpp \
    tcpserver.cpp \
    routedmessagetests.cpp \
    routertests.cpp \
    routesubscriptiontests.cpp \
    clienttests.cpp

HEADERS += \
    willtests.h \
    messagetests.h \
    frametests.h \
    networktests.h \
    tcpserver.h \
    routedmessagetests.h \
    routertests.h \
    routesubscriptiontests.h \
    clienttests.h

INCLUDEPATH += ..
LIBS += -L.. -lqmqtt
