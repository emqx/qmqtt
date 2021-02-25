INCLUDEPATH += $$PWD/src/mqtt

HEADERS += \
    $$PWD/src/mqtt/qmqtt.h \
    $$PWD/src/mqtt/qmqtt_client.h \
    $$PWD/src/mqtt/qmqtt_client_p.h \
    $$PWD/src/mqtt/qmqtt_frame.h \
    $$PWD/src/mqtt/qmqtt_global.h \
    $$PWD/src/mqtt/qmqtt_message.h \
    $$PWD/src/mqtt/qmqtt_message_p.h \
    $$PWD/src/mqtt/qmqtt_network_p.h \
    $$PWD/src/mqtt/qmqtt_networkinterface.h \
    $$PWD/src/mqtt/qmqtt_routedmessage.h \
    $$PWD/src/mqtt/qmqtt_router.h \
    $$PWD/src/mqtt/qmqtt_routesubscription.h \
    $$PWD/src/mqtt/qmqtt_socket_p.h \
    $$PWD/src/mqtt/qmqtt_socketinterface.h \
    $$PWD/src/mqtt/qmqtt_timer_p.h \
    $$PWD/src/mqtt/qmqtt_timerinterface.h \
    $$PWD/src/mqtt/qmqtt_ssl_socket_p.h

SOURCES += \
    $$PWD/src/mqtt/qmqtt_client.cpp \
    $$PWD/src/mqtt/qmqtt_client_p.cpp \
    $$PWD/src/mqtt/qmqtt_frame.cpp \
    $$PWD/src/mqtt/qmqtt_message.cpp \
    $$PWD/src/mqtt/qmqtt_network.cpp \
    $$PWD/src/mqtt/qmqtt_router.cpp \
    $$PWD/src/mqtt/qmqtt_routesubscription.cpp \
    $$PWD/src/mqtt/qmqtt_socket.cpp \
    $$PWD/src/mqtt/qmqtt_timer.cpp \
    $$PWD/src/mqtt/qmqtt_ssl_socket.cpp

QMQTT_WEBSOCKETS {
    PRIVATE_HEADERS += \
        $$PWD/src/mqtt/qmqtt_websocket_p.h \
        $$PWD/src/mqtt/qmqtt_websocketiodevice_p.h

    SOURCES += \
        $$PWD/src/mqtt/qmqtt_websocket.cpp \
        $$PWD/src/mqtt/qmqtt_websocketiodevice.cpp
}
