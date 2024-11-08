/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockfloatingpreview.h"

QX_DOCK_BEGIN_NAMESPACE

class DockFloatingPreviewPrivate
{
public:
    QX_DECLARE_PUBLIC(DockFloatingPreview)
public:
    DockFloatingPreviewPrivate();
};

DockFloatingPreviewPrivate::DockFloatingPreviewPrivate()
{

}

DockFloatingPreview::DockFloatingPreview(QWidget *parent)
    : QWidget(parent)
{
    QX_INIT_PRIVATE(DockFloatingPreview);
}

DockFloatingPreview::~DockFloatingPreview()
{
    QX_FINI_PRIVATE()
}

void DockFloatingPreview::startFloating(const QPoint &dragStartMousePos, const QSize &size,
                                        Qx::DockDragState dragState, QWidget *mouseEventHandler)
{

}

void DockFloatingPreview::finishDragging()
{

}

void DockFloatingPreview::moveFloating()
{

}

QX_DOCK_END_NAMESPACE
