QT += widgets testlib network
QT -= gui
TARGET = qmqtt_unittests
DEFINES += QMQTT_LIBRARY_TESTS
CONFIG += testcase

SOURCES += \
    testmain.cpp \
    willtests.cpp \
    messagetests.cpp \
    frametests.cpp \
    networktests.cpp \
    tcpserver.cpp

HEADERS += \
    willtests.h \
    messagetests.h \
    frametests.h \
    networktests.h \
    tcpserver.h

INCLUDEPATH += ..
LIBS += -L.. -lqmqtt
