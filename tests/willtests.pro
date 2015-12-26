QT += widgets testlib
QT -= gui
DEFINES += QMQTT_LIBRARY_TESTS
TARGET = willtests
CONFIG += testcase
SOURCES += willtests.cpp
INCLUDEPATH += ..
LIBS += -L.. -lqmqtt
