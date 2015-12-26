QT += widgets testlib
QT -= gui
TARGET = routedmessagetests
DEFINES += QMQTT_LIBRARY_TESTS
CONFIG += testcase
SOURCES += routedmessagetests.cpp
INCLUDEPATH += ..
LIBS += -L.. -lqmqtt
