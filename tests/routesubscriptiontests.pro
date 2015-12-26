QT += widgets testlib
QT -= gui
TARGET = routesubscriptiontests
DEFINES += QMQTT_LIBRARY_TESTS
CONFIG += testcase
SOURCES += routesubscriptiontests.cpp
INCLUDEPATH += ..
LIBS += -L.. -lqmqtt
