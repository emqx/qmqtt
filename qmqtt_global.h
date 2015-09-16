/*
 * qmqtt_global.h - qmqtt libray global
 *
 * Copyright (c) 2013  Ery Lee <ery.lee at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of mqttc nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef QMQTT_GLOBAL_H
#define QMQTT_GLOBAL_H

//#include <QtCore/qglobal.h>
#if defined(QMQTT_LIBRARY)
#  define QMQTTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QMQTTSHARED_EXPORT Q_DECL_IMPORT
#endif

#define QMQTT_VERSION "0.2.0"

/*
#define P_DECLARE_PRIVATE(Class) \
    friend class Class##Private; \
    inline Class##Private* pd_func() { return reinterpret_cast<Class##Private *>(this->pd_ptr); } \
    inline const Class##Private* pd_func() const { return reinterpret_cast<const Class##Private *>(this->pd_ptr); }

#define P_DECLARE_PUBLIC(Class) \
    inline Class* pq_func() { return static_cast<Class *>(this->pq_ptr); } \
    inline const Class* pq_func() const { return static_cast<const Class *>(this->pq_ptr); } \
    friend class Class;

#define P_D(Class) Class##Private * const d = this->pd_func()
#define P_Q(Class) Class * const q = this->pq_func()
*/
#endif // QMQTT_GLOBAL_H

