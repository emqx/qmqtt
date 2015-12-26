QT += widgets testlib
QT -= gui
DEFINES += QMQTT_LIBRARY_TESTS
TARGET = frametests
CONFIG += testcase
SOURCES += frametests.cpp
INCLUDEPATH += ..
LIBS += -L.. -lqmqtt
