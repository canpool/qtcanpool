/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockfloatingcontainer.h"

QX_DOCK_BEGIN_NAMESPACE

class DockFloatingContainerPrivate
{
public:
    QX_DECLARE_PUBLIC(DockFloatingContainer)
public:
    DockFloatingContainerPrivate();
};

DockFloatingContainerPrivate::DockFloatingContainerPrivate()
{

}

DockFloatingContainer::DockFloatingContainer(QWidget *parent)
    : QWidget(parent)
{
    QX_INIT_PRIVATE(DockFloatingContainer);
}

DockFloatingContainer::~DockFloatingContainer()
{
    QX_FINI_PRIVATE()
}

void QxDock::DockFloatingContainer::startFloating(const QPoint &dragStartMousePos, const QSize &size,
                                                  Qx::DockDragState dragState, QWidget *mouseEventHandler)
{

}

void DockFloatingContainer::finishDragging()
{

}

void DockFloatingContainer::moveFloating()
{

}

QX_DOCK_END_NAMESPACE
