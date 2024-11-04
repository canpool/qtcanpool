/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockpanel.h"

QX_DOCK_BEGIN_NAMESPACE

class DockPanelPrivate
{
public:
    QX_DECLARE_PUBLIC(DockPanel)
public:
    DockPanelPrivate();
};

DockPanelPrivate::DockPanelPrivate()
{
}

DockPanel::DockPanel(QWidget *parent)
    : QWidget{parent}
{
    QX_INIT_PRIVATE(DockPanel)
}

DockPanel::~DockPanel()
{
    QX_FINI_PRIVATE()
}

QX_DOCK_END_NAMESPACE
