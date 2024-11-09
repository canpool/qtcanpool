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
    Qx::DockWidgetAreas m_allowedAreas = Qx::InvalidDockWidgetArea;
    Qx::DockWidgetArea m_lastLocation = Qx::InvalidDockWidgetArea;
    QPointer<QWidget> m_targetWidget;
    bool m_dropPreviewEnabled = true;
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

void DockOverlay::setAllowedAreas(Qx::DockWidgetAreas areas)
{
    Q_D(DockOverlay);
    if (areas == d->m_allowedAreas) {
        return;
    }
    d->m_allowedAreas = areas;
}

Qx::DockWidgetAreas DockOverlay::allowedAreas() const
{
    Q_D(const DockOverlay);
    return d->m_allowedAreas;
}

Qx::DockWidgetArea DockOverlay::dropAreaUnderCursor() const
{
    return Qx::InvalidDockWidgetArea;
}

Qx::DockWidgetArea DockOverlay::showOverlay(QWidget *target)
{
    Q_D(DockOverlay);
    if (d->m_targetWidget == target) {
        // Hint: We could update geometry of overlay here.
        Qx::DockWidgetArea da = dropAreaUnderCursor();
        if (da != d->m_lastLocation) {
            repaint();
            d->m_lastLocation = da;
        }
        return da;
    }

    d->m_targetWidget = target;
    d->m_lastLocation = Qx::InvalidDockWidgetArea;

    // Move it over the target.
    hide();
    resize(target->size());
    QPoint TopLeft = target->mapToGlobal(target->rect().topLeft());
    move(TopLeft);
    show();

    return dropAreaUnderCursor();
}

void DockOverlay::hideOverlay()
{
    hide();
}

void DockOverlay::enableDropPreview(bool enable)
{
    Q_D(DockOverlay);
    d->m_dropPreviewEnabled = enable;
    update();
}

QX_DOCK_END_NAMESPACE
