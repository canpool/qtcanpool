/**
 * Copyright (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: Apache-2.0
**/
#ifndef WIDGETITEMDELEGATE_P_H
#define WIDGETITEMDELEGATE_P_H

//
//  W A R N I N G !!!
//  -----------------
//
// This file is not part of the QxWindow API. It is used purely as an
// implementation detail. This header file may change from version to
// version without notice, or may even be removed.
//

#include "qxwindow_global.h"
#include "windowitemdelegate_p.h"
#include <QtCore/QObject>
#include <QtGui/QWindow>

QX_WINDOW_BEGIN_NAMESPACE

class QX_WINDOW_EXPORT WidgetItemDelegate : public WindowItemDelegate
{
public:
    WidgetItemDelegate();
    ~WidgetItemDelegate() override;
public:
    QWindow *window(const QObject *obj) const override;
    bool isEnabled(const QObject *obj) const override;
    bool isVisible(const QObject *obj) const override;
    QRect mapGeometryToScene(const QObject *obj) const override;
    QObject *childAt(const QObject *obj, const QPoint &pos) const override;

    QWindow *hostWindow(const QObject *host) const override;
    bool isHostSizeFixed(const QObject *host) const override;
    bool isWindowActive(const QObject *host) const override;
    Qt::WindowStates getWindowState(const QObject *host) const override;
    Qt::WindowFlags getWindowFlags(const QObject *host) const override;

    void resetQtGrabbedControl(QObject *host) const override;
    void setWindowState(QObject *host, Qt::WindowStates state) const override;
    void setCursorShape(QObject *host, Qt::CursorShape shape) const override;
    void restoreCursorShape(QObject *host) const override;
    void setWindowFlags(QObject *host, Qt::WindowFlags flags) const override;
    void setWindowVisible(QObject *host, bool visible) const override;
    void bringWindowToTop(QObject *host) const override;
};

QX_WINDOW_END_NAMESPACE

#endif   // WIDGETITEMDELEGATE_P_H
