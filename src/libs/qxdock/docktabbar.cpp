/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "docktabbar.h"
#include "dockpanel.h"

QX_DOCK_BEGIN_NAMESPACE

class DockTabBarPrivate
{
public:
    QX_DECLARE_PUBLIC(DockTabBar)
public:
    DockTabBarPrivate();
public:
    DockPanel *m_panel = nullptr;
};

DockTabBarPrivate::DockTabBarPrivate()
{

}

DockTabBar::DockTabBar(DockPanel *parent)
    : QScrollArea(parent)
{
    QX_INIT_PRIVATE(DockTabBar)
    Q_D(DockTabBar);
    d->m_panel = parent;
}

DockTabBar::~DockTabBar()
{
    QX_FINI_PRIVATE()
}

QX_DOCK_END_NAMESPACE
