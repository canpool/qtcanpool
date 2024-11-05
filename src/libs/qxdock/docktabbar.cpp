/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "docktabbar.h"

QX_DOCK_BEGIN_NAMESPACE

class DockTabBarPrivate
{
public:
    QX_DECLARE_PUBLIC(DockTabBar)
public:
    DockTabBarPrivate();
};

DockTabBarPrivate::DockTabBarPrivate()
{

}

DockTabBar::DockTabBar(QWidget *parent)
    : QWidget(parent)
{
    QX_INIT_PRIVATE(DockTabBar);
}

DockTabBar::~DockTabBar()
{
    QX_FINI_PRIVATE()
}

QX_DOCK_END_NAMESPACE
