/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockoverlay.h"

QX_DOCK_BEGIN_NAMESPACE

class DockOverlayPrivate
{
public:
    QX_DECLARE_PUBLIC(DockOverlay)
public:
    DockOverlayPrivate();
};

DockOverlayPrivate::DockOverlayPrivate()
{

}

DockOverlay::DockOverlay(QWidget *parent)
    : QWidget(parent)
{
    QX_INIT_PRIVATE(DockOverlay);
}

DockOverlay::~DockOverlay()
{
    QX_FINI_PRIVATE()
}

QX_DOCK_END_NAMESPACE
