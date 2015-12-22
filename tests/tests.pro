QT += widgets testlib network
QT -= gui
TARGET = qmqtt_tests
DEFINES += QMQTT_LIBRARY_TESTS
CONFIG += testcase

include(tests.pri)
SOURCES += $${TEST_SOURCES}
HEADERS += $${TEST_HEADERS}
INCLUDEPATH += ..
LIBS += -L.. -lqmqtt
