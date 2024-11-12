/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include <QObject>
#include <QWindow>

QX_DOCK_BEGIN_NAMESPACE

class DockWindow;
class DockFloatingContainer;
class DockTab;
class DockWidget;

class DockFocusControllerPrivate;

class QX_DOCK_EXPORT DockFocusController : public QObject
{
    Q_OBJECT
public:
    explicit DockFocusController(DockWindow *window);
    virtual ~DockFocusController();

    void notifyDockAreaRelocation(QWidget *droppedWidget);
    void notifyFloatingWidgetDrop(DockFloatingContainer *floatingWidget);

    void setDockWidgetTabFocused(DockTab *tab);
    void setDockWidgetTabPressed(bool Value);

    void clearDockWidgetFocus(DockWidget *w);

public Q_SLOTS:
    void setDockWidgetFocused(DockWidget *focusedNow);

private Q_SLOTS:
    void onApplicationFocusChanged(QWidget *focusedOld, QWidget *focusedNow);
    void onFocusWindowChanged(QWindow *focusWindow);

private:
    QX_DECLARE_PRIVATE(DockFocusController)
};

QX_DOCK_END_NAMESPACE
