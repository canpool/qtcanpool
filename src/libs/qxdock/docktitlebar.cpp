/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "docktitlebar.h"

QX_DOCK_BEGIN_NAMESPACE

class DockTitleBarPrivate
{
public:
    QX_DECLARE_PUBLIC(DockTitleBar)
public:
    DockTitleBarPrivate();
};

DockTitleBarPrivate::DockTitleBarPrivate()
{

}

DockTitleBar::DockTitleBar(QWidget *parent)
    : QWidget(parent)
{
    QX_INIT_PRIVATE(DockTitleBar);
}

DockTitleBar::~DockTitleBar()
{
    QX_FINI_PRIVATE()
}

QX_DOCK_END_NAMESPACE
