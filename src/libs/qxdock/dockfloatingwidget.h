/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include <QWidget>

QX_DOCK_BEGIN_NAMESPACE

class QX_DOCK_EXPORT DockFloatingWidget
{
public:
    virtual ~DockFloatingWidget() = default;

    virtual void startFloating(const QPoint &dragStartMousePos, const QSize &size,
        Qx::DockDragState dragState, QWidget *mouseEventHandler) = 0;

    virtual void moveFloating() = 0;
    virtual void finishDragging() = 0;
};

QX_DOCK_END_NAMESPACE
