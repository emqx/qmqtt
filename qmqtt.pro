#-------------------------------------------------
#
# Project created by QtCreator 2013-03-16T14:26:50
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = qmqtt
TEMPLATE = lib

DEFINES += QMQTT_LIBRARY

SOURCES += qmqtt.cpp \
    qmqtt_message.cpp \
    qmqtt_will.cpp

HEADERS += qmqtt.h\
        qmqtt_global.h \
    qmqtt_message.h \
    qmqtt_will.h \
    qmqtt_packet.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
