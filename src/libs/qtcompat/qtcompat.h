/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QTCOMPAT_H
#define QTCOMPAT_H

#include <QtCore/QPoint>
#include <QtGui/QHoverEvent>
#include <QtGui/QMouseEvent>

/*
 * Small shims for Qt API that changed between Qt 5 and Qt 6.
 *
 * Qt 6 replaced the integer based pointer accessors of QMouseEvent and
 * QHoverEvent with the QPointF based ones inherited from QSinglePointEvent and
 * deprecated the old names, while Qt 5.15 only offers the old integer ones.
 * These helpers pick whichever exists so that call sites stay free of
 * #if QT_VERSION noise. They are header-only on purpose: src/libs is on the
 * include path of both the CMake and the frozen qmake build.
 */

namespace QtCanpoolCompat
{

inline QPoint globalMousePos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->globalPosition().toPoint();
#else
    return event->globalPos();
#endif
}

inline QPoint hoverPos(const QHoverEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position().toPoint();
#else
    return event->pos();
#endif
}

}   // namespace QtCanpoolCompat

#endif   // QTCOMPAT_H
