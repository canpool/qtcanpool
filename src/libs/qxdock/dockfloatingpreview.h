/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include "dockfloatingwidget.h"
#include <QWidget>

QX_DOCK_BEGIN_NAMESPACE

class DockFloatingPreviewPrivate;

class QX_DOCK_EXPORT DockFloatingPreview : public QWidget
                                         , public DockFloatingWidget
{
    Q_OBJECT
public:
    explicit DockFloatingPreview(QWidget *parent = nullptr);
    virtual ~DockFloatingPreview();

public:
    virtual void startFloating(const QPoint &dragStartMousePos, const QSize &size,
        Qx::DockDragState dragState, QWidget *mouseEventHandler) override;
    virtual void finishDragging() override;
    void moveFloating() override;

Q_SIGNALS:
    void draggingCanceled();

private:
    QX_DECLARE_PRIVATE(DockFloatingPreview)
};

QX_DOCK_END_NAMESPACE