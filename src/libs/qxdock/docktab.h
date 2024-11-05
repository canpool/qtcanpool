/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include <QWidget>

QX_DOCK_BEGIN_NAMESPACE

class DockWidget;
class DockPanel;

class DockTabPrivate;

class QX_DOCK_EXPORT DockTab : public QWidget
{
    Q_OBJECT
public:
    explicit DockTab(DockWidget *w, QWidget *parent = nullptr);
    virtual ~DockTab();

    void setDockPanel(DockPanel *panel);

private:
    QX_DECLARE_PRIVATE(DockTab)
};

QX_DOCK_END_NAMESPACE
