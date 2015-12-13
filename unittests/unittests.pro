QT += widgets testlib
QT -= gui
TARGET = qmqtt_unittests
DEFINES += QMQTT_LIBRARY_TESTS

SOURCES += \
    testmain.cpp

HEADERS += \
    sampletest2.h \
    sampletest.h
