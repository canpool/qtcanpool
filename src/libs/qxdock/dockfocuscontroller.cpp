/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "dockfocuscontroller.h"

QX_DOCK_BEGIN_NAMESPACE

class DockFocusControllerPrivate
{
public:
    QX_DECLARE_PUBLIC(DockFocusController)
public:
    DockFocusControllerPrivate();
};

DockFocusControllerPrivate::DockFocusControllerPrivate()
{
}

DockFocusController::DockFocusController(QObject *parent)
    : QObject(parent)
{
    QX_INIT_PRIVATE(DockFocusController);
}

DockFocusController::~DockFocusController()
{
    QX_FINI_PRIVATE();
}

QX_DOCK_END_NAMESPACE
