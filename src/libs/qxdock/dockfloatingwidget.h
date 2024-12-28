/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include <QWidget>

QX_DOCK_BEGIN_NAMESPACE

/**
 * Pure virtual interface for floating widgets.
 * This interface is used for opaque and non-opaque undocking. If opaque
 * undocking is used, the a real DockFloatingContainer widget will be created
 */
class QX_DOCK_EXPORT DockFloatingWidget
{
public:
    virtual ~DockFloatingWidget() = default;

    /**
     * Starts floating.
     * This function should get called typically from a mouse press event handler
     */
    virtual void startFloating(const QPoint &dragStartMousePos, const QSize &size,
        Qx::DockDragState dragState, QWidget *mouseEventHandler) = 0;

    /**
     * Moves the widget to a new position relative to the position given when
     * startFloating() was called.
     * This function should be called from a mouse move event handler to
     * move the floating widget on mouse move events.
     */
    virtual void moveFloating() = 0;

    /**
     * Tells the widget that to finish dragging if the mouse is released.
     * This function should be called from a mouse release event handler
     * to finish the dragging
     */
    virtual void finishDragging() = 0;
};

QX_DOCK_END_NAMESPACE
