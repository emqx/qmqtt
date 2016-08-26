QT += network
QT -= gui

TARGET = qmqtt_example

DEFINES += QMQTT_EXAMPLE
INCLUDEPATH += ../src/mqtt
LIBS += -L../src/mqtt -L../src/mqtt/debug -L../src/mqtt/release -lqmqtt

SOURCES = example.cpp
