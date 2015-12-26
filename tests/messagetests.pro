QT += widgets testlib
QT -= gui
DEFINES += QMQTT_LIBRARY_TESTS
TARGET = messagetests
CONFIG += testcase
SOURCES += messagetests.cpp
INCLUDEPATH += ..
LIBS += -L.. -lqmqtt
