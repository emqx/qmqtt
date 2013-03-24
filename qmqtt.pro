#-------------------------------------------------
#
# Project created by QtCreator 2013-03-20T14:30:19
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = qmqtt
TEMPLATE = lib

DEFINES += QMQTT_LIBRARY

SOURCES += qmqtt_client.cpp \
    qmqtt_network.cpp \
    qmqtt_frame.cpp \
    qmqtt_client_p.cpp \
    qmqtt_message.cpp \
    qmqtt_will.cpp

HEADERS += qmqtt_client.h\
        qmqtt_global.h \
    qmqtt_network.h \
    qmqtt_frame.h \
    qmqtt_client_p.h \
    qmqtt_message.h \
    qmqtt_will.h \
    qmqtt.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
