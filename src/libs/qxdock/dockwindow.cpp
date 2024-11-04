/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockwindow.h"

QX_DOCK_BEGIN_NAMESPACE

class DockWindowPrivate
{
public:
    QX_DECLARE_PUBLIC(DockWindow)
public:
    DockWindowPrivate();
};

DockWindowPrivate::DockWindowPrivate()
{
}

DockWindow::DockWindow(QWidget *parent)
    : DockContainer{parent}
{
    QX_INIT_PRIVATE(DockWindow)
}

DockWindow::~DockWindow()
{
    QX_FINI_PRIVATE()
}

QX_DOCK_END_NAMESPACE
