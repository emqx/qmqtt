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
    qmqtt_router.cpp \
    qmqtt_routesubscription.cpp \
    qmqtt_routedmessage.cpp \
    qmqtt_message_p.cpp \
    qmqtt_socket.cpp \
    qmqtt_timer.cpp \
    qmqtt_ssl_socket.cpp \
    qmqtt_ssl_network.cpp

HEADERS += qmqtt_client.h\
    qmqtt_global.h \
    qmqtt_network.h \
    qmqtt_frame.h \
    qmqtt_client_p.h \
    qmqtt_message.h \
    qmqtt_router.h \
    qmqtt.h \
    qmqtt_routesubscription.h \
    qmqtt_routedmessage.h \
    qmqtt_networkinterface.h \
    qmqtt_message_p.h \
    qmqtt_socketinterface.h \
    qmqtt_socket.h \
    qmqtt_timer.h \
    qmqtt_timerinterface.h \
    qmqtt_ssl_socket.h \
    qmqtt_ssl_network.h

isEmpty(PREFIX) {
    contains(MEEGO_EDITION,harmattan) {
        PREFIX = /usr
    } else:unix:!symbian {
        maemo5 {
            PREFIX = /opt/usr
        } else {
            PREFIX = /usr/local
        }
    } else {
        PREFIX = $$[QT_INSTALL_PREFIX]
    }
}

headers.files = $$HEADERS
headers.path = $$PREFIX/include/qmqtt
target.path = $$PREFIX/lib
INSTALLS += headers target

OTHER_FILES += \
    qmqtt.pri
