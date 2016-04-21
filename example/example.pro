QT += widgets network
QT -= gui

TARGET = qmqtt_example

DEFINES += QMQTT_EXAMPLE
INCLUDEPATH += ../src
LIBS += -L../src -L../src/debug -L../src/release -lqmqtt

SOURCES = example.cpp
