/**
 * Copyright (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: Apache-2.0
**/
#ifndef WINDOWITEMDELEGATE_P_H
#define WINDOWITEMDELEGATE_P_H

//
//  W A R N I N G !!!
//  -----------------
//
// This file is not part of the QxWindow API. It is used purely as an
// implementation detail. This header file may change from version to
// version without notice, or may even be removed.
//

#include "qxwindow_global.h"
#include <QtCore/QObject>
#include <QtCore/QPoint>
#include <QtGui/QWindow>

QX_WINDOW_BEGIN_NAMESPACE

class QX_WINDOW_EXPORT WindowItemDelegate
{
public:
    WindowItemDelegate();
    virtual ~WindowItemDelegate();
public:
    // Item property query
    virtual QWindow *window(const QObject *obj) const = 0;
    virtual bool isEnabled(const QObject *obj) const = 0;
    virtual bool isVisible(const QObject *obj) const = 0;
    virtual QRect mapGeometryToScene(const QObject *obj) const = 0;
    virtual QObject *childAt(const QObject *obj, const QPoint &pos) const = 0;

    // Host property query
    virtual QWindow *hostWindow(const QObject *host) const = 0;
    virtual bool isHostSizeFixed(const QObject *host) const = 0;
    virtual bool isWindowActive(const QObject *host) const = 0;
    virtual Qt::WindowStates getWindowState(const QObject *host) const = 0;
    virtual Qt::WindowFlags getWindowFlags(const QObject *host) const = 0;

    // Callbacks
    virtual void resetQtGrabbedControl(QObject *host) const;
    virtual void setWindowState(QObject *host, Qt::WindowStates state) const = 0;
    virtual void setCursorShape(QObject *host, Qt::CursorShape shape) const = 0;
    virtual void restoreCursorShape(QObject *host) const = 0;
    virtual void setWindowFlags(QObject *host, Qt::WindowFlags flags) const = 0;
    virtual void setWindowVisible(QObject *host, bool visible) const = 0;
    virtual void bringWindowToTop(QObject *host) const = 0;
private:
    Q_DISABLE_COPY(WindowItemDelegate)
};

QX_WINDOW_END_NAMESPACE

#endif   // WINDOWITEMDELEGATE_P_H
