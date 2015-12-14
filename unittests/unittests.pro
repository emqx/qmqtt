QT += widgets testlib
QT -= gui
TARGET = qmqtt_unittests
DEFINES += QMQTT_LIBRARY_TESTS
CONFIG += testcase

SOURCES += \
    testmain.cpp \
    willtests.cpp \
    messagetests.cpp

HEADERS += \
    willtests.h \
    messagetests.h

INCLUDEPATH += ..
LIBS += -L.. -lqmqtt
