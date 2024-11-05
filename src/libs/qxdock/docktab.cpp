/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "docktab.h"

QX_DOCK_BEGIN_NAMESPACE

class DockTabPrivate
{
public:
    QX_DECLARE_PUBLIC(DockTab)
public:
    DockTabPrivate();
};

DockTabPrivate::DockTabPrivate()
{

}

DockTab::DockTab(QWidget *parent)
    : QWidget(parent)
{
    QX_INIT_PRIVATE(DockTab);
}

DockTab::~DockTab()
{
    QX_FINI_PRIVATE()
}

QX_DOCK_END_NAMESPACE
