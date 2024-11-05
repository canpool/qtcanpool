/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock_global.h"
#include <QWidget>

QX_DOCK_BEGIN_NAMESPACE

class DockWindow;
class DockContainer;

class DockWidgetPrivate;

class QX_DOCK_EXPORT DockWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DockWidget(QWidget *parent = nullptr);
    ~DockWidget();

protected:
    void setDockWindow(DockWindow *window);
private:
    QX_DECLARE_PRIVATE(DockWidget)
    friend class DockContainer;
};

QX_DOCK_END_NAMESPACE
