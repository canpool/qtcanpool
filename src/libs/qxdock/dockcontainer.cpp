/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockcontainer.h"

QX_DOCK_BEGIN_NAMESPACE

class DockContainerPrivate
{
public:
    QX_DECLARE_PUBLIC(DockContainer)
public:
    DockContainerPrivate();
};

DockContainerPrivate::DockContainerPrivate()
{

}

DockContainer::DockContainer(QWidget *parent)
    : QWidget(parent)
{
    QX_INIT_PRIVATE(DockContainer);
}

DockContainer::~DockContainer()
{
    QX_FINI_PRIVATE()
}

QX_DOCK_END_NAMESPACE
