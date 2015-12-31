QT += widgets network
QT -= gui

TARGET = qmqtt_example

DEFINES += QMQTT_EXAMPLE
INCLUDEPATH += ../src
LIBS += -L../src -lqmqtt

SOURCES = example.cpp
