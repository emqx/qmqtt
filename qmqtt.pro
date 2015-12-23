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
    qmqtt_router.cpp \
    qmqtt_will.cpp \
    qmqtt_routesubscription.cpp \
    qmqtt_routedmessage.cpp

HEADERS += qmqtt_client.h\
    qmqtt_global.h \
    qmqtt_network.h \
    qmqtt_frame.h \
    qmqtt_client_p.h \
    qmqtt_message.h \
    qmqtt_will.h \
    qmqtt_router.h \
    qmqtt.h \
    qmqtt_routesubscription.h \
    qmqtt_routedmessage.h

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

TEST_FILES += \
    tests/willtests.cpp \
    tests/willtests.pro \
    tests/messagetests.cpp \
    tests/messagetests.pro \
    tests/frametests.cpp \
    tests/frametests.pro \
    tests/routedmessagetests.cpp \
    tests/routedmessagetests.pro \
    tests/routertests.cpp \
    tests/routertests.pro \
    tests/routesubscriptiontests.cpp \
    tests/routesubscriptiontests.pro \
    tests/networktests.cpp \
    tests/networktests.pro \
    tests/clienttests.cpp \
    tests/clienttests.pro \
    tests/tcpserver.cpp \
    tests/tcpserver.h

include(tests/tests.pri)
