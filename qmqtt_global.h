#ifndef QMQTT_GLOBAL_H
#define QMQTT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QMQTT_LIBRARY)
#  define QMQTTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QMQTTSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QMQTT_GLOBAL_H
