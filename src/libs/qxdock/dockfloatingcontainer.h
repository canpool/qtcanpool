/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include "dockfloatingwidget.h"

#include <QWidget>
#define DockFloatingContainerBase QWidget

QX_DOCK_BEGIN_NAMESPACE

class DockWindow;
class DockPanel;

class DockFloatingContainerPrivate;

class QX_DOCK_EXPORT DockFloatingContainer : public DockFloatingContainerBase
                                           , public DockFloatingWidget
{
    Q_OBJECT
public:
    using Super = DockFloatingContainerBase;
public:
    explicit DockFloatingContainer(DockWindow *window);
    explicit DockFloatingContainer(DockPanel *panel);
    virtual ~DockFloatingContainer();

private Q_SLOTS:
    void onDockAreasAddedOrRemoved();

protected:
    virtual void startFloating(const QPoint &dragStartMousePos, const QSize &size,
        Qx::DockDragState dragState, QWidget *mouseEventHandler) override;
    virtual void finishDragging() override;
    void moveFloating() override;

private:
    QX_DECLARE_PRIVATE(DockFloatingContainer)
};

QX_DOCK_END_NAMESPACE
