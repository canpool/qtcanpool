/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockoverlay.h"

#include <QPainter>

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
    QRect m_dropAreaRect;
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

void DockOverlay::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    Q_D(DockOverlay);

    // Draw rect based on location
    if (!d->m_dropPreviewEnabled) {
        d->m_dropAreaRect = QRect();
        return;
    }

    QRect r = rect();
    const Qx::DockWidgetArea da = dropAreaUnderCursor();
    double factor = (DockOverlay::ContainerOverlayMode == d->m_mode) ? 3 : 2;

    switch (da)
    {
    case Qx::TopDockWidgetArea: r.setHeight(r.height() / factor); break;
    case Qx::RightDockWidgetArea: r.setX(r.width() * (1 - 1 / factor)); break;
    case Qx::BottomDockWidgetArea: r.setY(r.height() * (1 - 1 / factor)); break;
    case Qx::LeftDockWidgetArea: r.setWidth(r.width() / factor); break;
    case Qx::CenterDockWidgetArea: r = rect();break;
    default: return;
    }

    QPainter painter(this);
    QColor color = palette().color(QPalette::Active, QPalette::Highlight);
    QPen pen = painter.pen();
    pen.setColor(color.darker(120));
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(1);
    pen.setCosmetic(true);
    painter.setPen(pen);
    color = color.lighter(130);
    color.setAlpha(64);
    painter.setBrush(color);
    painter.drawRect(r.adjusted(0, 0, -1, -1));

    d->m_dropAreaRect = r;
}

void DockOverlay::showEvent(QShowEvent *e)
{
    Super::showEvent(e);
}

void DockOverlay::hideEvent(QHideEvent *e)
{
    Super::hideEvent(e);
}

QX_DOCK_END_NAMESPACE
