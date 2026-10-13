/**
 * Copyright (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: Apache-2.0
 **/
#ifndef WINDOWKIT_P_H
#define WINDOWKIT_P_H

//
//  W A R N I N G !!!
//  -----------------
//
// This file is not part of the QxWindow API. It is used purely as an
// implementation detail. This header file may change from version to
// version without notice, or may even be removed.
//

#include "qxwindow_global.h"
#include <QtCore/QLoggingCategory>
#include <QtCore/QObject>
#include <QtGui/QMouseEvent>

Q_DECLARE_LOGGING_CATEGORY(qxWindowLog)

#define QXWINDOW_INFO     qCInfo(qxWindowLog) << "INFO:"
#define QXWINDOW_DEBUG    qCDebug(qxWindowLog) << "DEBUG:"
#define QXWINDOW_WARNING  qCWarning(qxWindowLog) << "WARNING:"
#define QXWINDOW_CRITICAL qCCritical(qxWindowLog) << "CRITICAL:"
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
#define QXWINDOW_FATAL qCFatal(qxWindowLog) << "FATAL:"
#endif

#define MAKE_RGB_COLOR(r, g, b) ((quint32)(((r) & 0xFF) << 16) | (((g) & 0xFF) << 8) | ((b) & 0xFF))

#define MAKE_RGBA_COLOR(r, g, b, a)                                                                                    \
    ((quint32)(((a) & 0xFF) << 24) | (((r) & 0xFF) << 16) | (((g) & 0xFF) << 8) | ((b) & 0xFF))

#if defined(Q_CC_MSVC)
#define QXWINDOW_NOINLINE __declspec(noinline)
#define QXWINDOW_INLINE   __forceinline
#define QXWINDOW_USED
#else
#define QXWINDOW_NOINLINE __attribute__((noinline))
#define QXWINDOW_INLINE   __attribute__((always_inline))
#define QXWINDOW_USED     __attribute__((used))
#endif

QX_WINDOW_BEGIN_NAMESPACE

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
using QT_NATIVE_EVENT_RESULT_TYPE = qintptr;
#else
using QT_NATIVE_EVENT_RESULT_TYPE = long;
#endif

namespace Private
{

class ObjectHelper : public QObject
{
public:
    static inline bool sendEvent(QObject *obj, QEvent *event)
    {
        return static_cast<ObjectHelper *>(obj)->event(event);
    }
};

}   // namespace Private

inline QPoint getMouseEventScenePos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->scenePosition().toPoint();
#else
    return event->windowPos().toPoint();
#endif
}

inline QPoint getMouseEventGlobalPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->globalPosition().toPoint();
#else
    return event->screenPos().toPoint();
#endif
}

QX_WINDOW_END_NAMESPACE

#endif   // WINDOWKIT_P_H
