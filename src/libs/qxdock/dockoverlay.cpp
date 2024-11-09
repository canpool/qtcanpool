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

public:
    DockOverlay::OverlayMode m_mode = DockOverlay::PanelOverlayMode;
};

DockOverlayPrivate::DockOverlayPrivate()
{

}

DockOverlay::DockOverlay(QWidget *parent, OverlayMode mode)
    : QWidget(parent)
{
    QX_INIT_PRIVATE(DockOverlay);
    Q_D(DockOverlay);

    d->m_mode = mode;

    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    setWindowOpacity(1);
    setWindowTitle("DockOverlay");
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);

    setVisible(false);
}

DockOverlay::~DockOverlay()
{
    QX_FINI_PRIVATE()
}

QX_DOCK_END_NAMESPACE
