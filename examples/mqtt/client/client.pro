TEMPLATE = app
TARGET = qmqtt_example
QT = core network mqtt

SOURCES += \
    example.cpp

target.path = $$[QT_INSTALL_EXAMPLES]/mqtt/client
INSTALLS += target
