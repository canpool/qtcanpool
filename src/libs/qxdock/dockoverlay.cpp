/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockoverlay.h"
#include "dockpanel.h"
#include "docktitlebar.h"
#include "docktabbar.h"
#include "dockcontainer.h"
#include "docksidebar.h"
#include "dockmanager.h"
#include "dockutils.h"

#include <QPainter>
#include <QGridLayout>
#include <QWindow>
#include <QLabel>
#include <QPointer>

QX_DOCK_BEGIN_NAMESPACE

static const int s_invalidTabIndex = -2;
static const int s_autoHideAreaWidth = 32;
static const int s_autoHideAreaMouseZone = 8;

class DockOverlayPrivate
{
public:
    QX_DECLARE_PUBLIC(DockOverlay)
public:
    DockOverlayPrivate();

    int sideBarOverlaySize(Qx::DockSideBarArea area);
    int sideBarMouseZone(Qx::DockSideBarArea area);
public:
    DockOverlay::OverlayMode m_mode = DockOverlay::PanelOverlayMode;
    Qx::DockWidgetAreas m_allowedAreas = Qx::InvalidDockWidgetArea;
    Qx::DockWidgetArea m_lastLocation = Qx::InvalidDockWidgetArea;
    QPointer<QWidget> m_targetWidget;
    bool m_dropPreviewEnabled = true;
    QRect m_dropAreaRect;
    int m_tabIndex = s_invalidTabIndex;
    DockOverlayCross *m_cross;
};

DockOverlayPrivate::DockOverlayPrivate()
{
}

int DockOverlayPrivate::sideBarOverlaySize(Qx::DockSideBarArea area)
{
    auto container = qobject_cast<DockContainer *>(m_targetWidget.data());
    auto sideBar = container->autoHideSideBar(area);
    if (!sideBar || !sideBar->isVisibleTo(container)) {
        return s_autoHideAreaWidth;
    } else {
        return (sideBar->orientation() == Qt::Horizontal) ? sideBar->height() : sideBar->width();
    }
}

int DockOverlayPrivate::sideBarMouseZone(Qx::DockSideBarArea area)
{
    auto container = qobject_cast<DockContainer *>(m_targetWidget.data());
    auto sideBar = container->autoHideSideBar(area);
    if (!sideBar || !sideBar->isVisibleTo(container)) {
        return s_autoHideAreaMouseZone;
    } else {
        return (sideBar->orientation() == Qt::Horizontal) ? sideBar->height() : sideBar->width();
    }
}

DockOverlay::DockOverlay(QWidget *parent, OverlayMode mode)
    : Super(parent)
{
    QX_INIT_PRIVATE(DockOverlay);
    Q_D(DockOverlay);

    d->m_mode = mode;
    d->m_cross = new DockOverlayCross(this);

    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    setWindowOpacity(1);
    setWindowTitle("DockOverlay");
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);

    d->m_cross->setVisible(false);
    setVisible(false);
}

DockOverlay::~DockOverlay()
{
    QX_FINI_PRIVATE();
}

void DockOverlay::setAllowedAreas(Qx::DockWidgetAreas areas)
{
    Q_D(DockOverlay);
    if (areas == d->m_allowedAreas) {
        return;
    }
    d->m_allowedAreas = areas;
    d->m_cross->reset();
}

Qx::DockWidgetAreas DockOverlay::allowedAreas() const
{
    Q_D(const DockOverlay);
    return d->m_allowedAreas;
}

Qx::DockWidgetArea DockOverlay::dropAreaUnderCursor()
{
    Q_D(DockOverlay);
    d->m_tabIndex = s_invalidTabIndex;
    if (!d->m_targetWidget) {
        return Qx::InvalidDockWidgetArea;
    }

    Qx::DockWidgetArea result = d->m_cross->cursorLocation();
    if (result != Qx::InvalidDockWidgetArea) {
        return result;
    }

    auto cursorPos = QCursor::pos();
    auto panel = qobject_cast<DockPanel *>(d->m_targetWidget.data());
    if (!panel && DockManager::autoHideConfigFlags().testFlag(DockManager::AutoHideFeatureEnabled)) {
        auto Rect = rect();
        const QPoint pos = mapFromGlobal(QCursor::pos());
        if ((pos.x() < d->sideBarMouseZone(Qx::DockSideBarLeft)) && d->m_allowedAreas.testFlag(Qx::LeftAutoHideArea)) {
            result = Qx::LeftAutoHideArea;
        } else if (pos.x() > (Rect.width() - d->sideBarMouseZone(Qx::DockSideBarRight)) &&
                   d->m_allowedAreas.testFlag(Qx::RightAutoHideArea)) {
            result = Qx::RightAutoHideArea;
        } else if (pos.y() < d->sideBarMouseZone(Qx::DockSideBarTop) &&
                   d->m_allowedAreas.testFlag(Qx::TopAutoHideArea)) {
            result = Qx::TopAutoHideArea;
        } else if (pos.y() > (Rect.height() - d->sideBarMouseZone(Qx::DockSideBarBottom)) &&
                   d->m_allowedAreas.testFlag(Qx::BottomAutoHideArea)) {
            result = Qx::BottomAutoHideArea;
        }

        auto sideBarArea = internal::toSideBarArea(result);
        if (sideBarArea != Qx::DockSideBarNone) {
            auto container = qobject_cast<DockContainer *>(d->m_targetWidget.data());
            auto sideBar = container->autoHideSideBar(sideBarArea);
            if (sideBar->isVisible()) {
                d->m_tabIndex = sideBar->tabInsertIndexAt(sideBar->mapFromGlobal(cursorPos));
            }
        }
        return result;
    } else if (!panel) {
        return result;
    }

    if (panel->allowedAreas().testFlag(Qx::CenterDockWidgetArea) && !panel->titleBar()->isHidden() &&
        panel->titleBarGeometry().contains(panel->mapFromGlobal(cursorPos))) {
        auto tabBar = panel->titleBar()->tabBar();
        d->m_tabIndex = tabBar->tabInsertIndexAt(tabBar->mapFromGlobal(cursorPos));
        return Qx::CenterDockWidgetArea;
    }

    return result;
}

int DockOverlay::tabIndexUnderCursor() const
{
    Q_D(const DockOverlay);
    return d->m_tabIndex;
}

Qx::DockWidgetArea DockOverlay::visibleDropAreaUnderCursor()
{
    Q_D(const DockOverlay);
    if (isHidden() || !d->m_dropPreviewEnabled) {
        return Qx::InvalidDockWidgetArea;
    } else {
        return dropAreaUnderCursor();
    }
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
    d->m_cross->updatePosition();
    d->m_cross->updateOverlayIcons();

    return dropAreaUnderCursor();
}

void DockOverlay::hideOverlay()
{
    Q_D(DockOverlay);
    hide();
    d->m_targetWidget.clear();
    d->m_lastLocation = Qx::InvalidDockWidgetArea;
    d->m_dropAreaRect = QRect();
}

void DockOverlay::enableDropPreview(bool enable)
{
    Q_D(DockOverlay);
    d->m_dropPreviewEnabled = enable;
    update();
}

QRect DockOverlay::dropOverlayRect() const
{
    Q_D(const DockOverlay);
    return d->m_dropAreaRect;
}

bool DockOverlay::event(QEvent *e)
{
    Q_D(DockOverlay);
    bool result = Super::event(e);
    if (e->type() == QEvent::Polish) {
        d->m_cross->setupOverlayCross(d->m_mode);
    }
    return result;
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

    switch (da) {
    case Qx::TopDockWidgetArea:
        r.setHeight(r.height() / factor);
        break;
    case Qx::RightDockWidgetArea:
        r.setX(r.width() * (1 - 1 / factor));
        break;
    case Qx::BottomDockWidgetArea:
        r.setY(r.height() * (1 - 1 / factor));
        break;
    case Qx::LeftDockWidgetArea:
        r.setWidth(r.width() / factor);
        break;
    case Qx::CenterDockWidgetArea:
        r = rect();
        break;
    case Qx::LeftAutoHideArea:
        r.setWidth(d->sideBarOverlaySize(Qx::DockSideBarLeft));
        break;
    case Qx::RightAutoHideArea:
        r.setX(r.width() - d->sideBarOverlaySize(Qx::DockSideBarRight));
        break;
    case Qx::TopAutoHideArea:
        r.setHeight(d->sideBarOverlaySize(Qx::DockSideBarTop));
        break;
    case Qx::BottomAutoHideArea:
        r.setY(r.height() - d->sideBarOverlaySize(Qx::DockSideBarBottom));
        break;
    default:
        return;
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
    Q_D(DockOverlay);
    d->m_cross->show();
    Super::showEvent(e);
}

void DockOverlay::hideEvent(QHideEvent *e)
{
    Q_D(DockOverlay);
    d->m_cross->hide();
    Super::hideEvent(e);
}

/* DockOverlayCrossPrivate */
class DockOverlayCrossPrivate
{
public:
    QX_DECLARE_PUBLIC(DockOverlayCross)
public:
    DockOverlayCrossPrivate();

    void init();

    QPoint areaGridPosition(const Qx::DockWidgetArea area);
    QColor defaultIconColor(DockOverlayCross::IconColor colorIndex);

    QColor iconColor(DockOverlayCross::IconColor colorIndex);

    qreal dropIndicatiorWidth(QLabel *l) const;

    QWidget *createDropIndicatorWidget(Qx::DockWidgetArea dockWidgetArea, DockOverlay::OverlayMode mode);
    void updateDropIndicatorIcon(QWidget *DropIndicatorWidget);
    QPixmap createHighDpiDropIndicatorPixmap(const QSizeF &size, Qx::DockWidgetArea dockWidgetArea,
                                             DockOverlay::OverlayMode mode);
public:
    DockOverlay *m_dockOverlay = nullptr;
    QGridLayout *m_gridLayout;
    DockOverlay::OverlayMode m_mode = DockOverlay::PanelOverlayMode;
    QHash<Qx::DockWidgetArea, QWidget *> m_dropIndicatorWidgets;
    QColor m_iconColors[DockOverlayCross::NIconColors];
    bool m_updateRequired = false;
    double m_lastDevicePixelRatio = 0.1;
};

DockOverlayCrossPrivate::DockOverlayCrossPrivate()
{
}

void DockOverlayCrossPrivate::init()
{
    Q_Q(DockOverlayCross);

    m_gridLayout = new QGridLayout();
    m_gridLayout->setSpacing(0);
    q->setLayout(m_gridLayout);
}

static int areaAlignment(const Qx::DockWidgetArea area)
{
    switch (area) {
    case Qx::TopDockWidgetArea:
        return (int)Qt::AlignHCenter | Qt::AlignBottom;
    case Qx::RightDockWidgetArea:
        return (int)Qt::AlignLeft | Qt::AlignVCenter;
    case Qx::BottomDockWidgetArea:
        return (int)Qt::AlignHCenter | Qt::AlignTop;
    case Qx::LeftDockWidgetArea:
        return (int)Qt::AlignRight | Qt::AlignVCenter;
    case Qx::CenterDockWidgetArea:
        return (int)Qt::AlignCenter;
    default:
        return Qt::AlignCenter;
    }
}

QPoint DockOverlayCrossPrivate::areaGridPosition(const Qx::DockWidgetArea area)
{
    if (DockOverlay::PanelOverlayMode == m_mode) {
        switch (area) {
        case Qx::TopDockWidgetArea:
            return QPoint(1, 2);
        case Qx::RightDockWidgetArea:
            return QPoint(2, 3);
        case Qx::BottomDockWidgetArea:
            return QPoint(3, 2);
        case Qx::LeftDockWidgetArea:
            return QPoint(2, 1);
        case Qx::CenterDockWidgetArea:
            return QPoint(2, 2);
        default:
            return QPoint();
        }
    } else {
        switch (area) {
        case Qx::TopDockWidgetArea:
            return QPoint(0, 2);
        case Qx::RightDockWidgetArea:
            return QPoint(2, 4);
        case Qx::BottomDockWidgetArea:
            return QPoint(4, 2);
        case Qx::LeftDockWidgetArea:
            return QPoint(2, 0);
        case Qx::CenterDockWidgetArea:
            return QPoint(2, 2);
        default:
            return QPoint();
        }
    }
}

QColor DockOverlayCrossPrivate::defaultIconColor(DockOverlayCross::IconColor colorIndex)
{
    Q_Q(DockOverlayCross);
    QPalette pal = q->palette();
    switch (colorIndex) {
    case DockOverlayCross::FrameColor:
        return pal.color(QPalette::Active, QPalette::Highlight);
    case DockOverlayCross::WindowBackgroundColor:
        return pal.color(QPalette::Active, QPalette::Base);
    case DockOverlayCross::OverlayColor: {
        QColor Color = pal.color(QPalette::Active, QPalette::Highlight);
        Color.setAlpha(64);
        return Color;
    } break;

    case DockOverlayCross::ArrowColor:
        return pal.color(QPalette::Active, QPalette::Base);
    case DockOverlayCross::ShadowColor:
        return QColor(0, 0, 0, 64);
    default:
        return QColor();
    }

    return QColor();
}

QColor DockOverlayCrossPrivate::iconColor(DockOverlayCross::IconColor colorIndex)
{
    QColor color = m_iconColors[colorIndex];
    if (!color.isValid()) {
        color = defaultIconColor(colorIndex);
        m_iconColors[colorIndex] = color;
    }
    return color;
}

qreal DockOverlayCrossPrivate::dropIndicatiorWidth(QLabel *l) const
{
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    Q_UNUSED(l)
    return 40;
#else
    return static_cast<qreal>(l->fontMetrics().height()) * 3.f;
#endif
}

QWidget *DockOverlayCrossPrivate::createDropIndicatorWidget(Qx::DockWidgetArea dockWidgetArea,
                                                            DockOverlay::OverlayMode mode)
{
    QLabel *l = new QLabel();
    l->setObjectName("DockWidgetAreaLabel");

    qreal metric = dropIndicatiorWidth(l);
    QSizeF size(metric, metric);

    l->setPixmap(createHighDpiDropIndicatorPixmap(size, dockWidgetArea, mode));
    l->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    l->setAttribute(Qt::WA_TranslucentBackground);
    l->setProperty("dockWidgetArea", dockWidgetArea);
    return l;
}

void DockOverlayCrossPrivate::updateDropIndicatorIcon(QWidget *dropIndicatorWidget)
{
    QLabel *l = qobject_cast<QLabel *>(dropIndicatorWidget);
    const qreal metric = dropIndicatiorWidth(l);
    const QSizeF size(metric, metric);

    int area = l->property("dockWidgetArea").toInt();
    l->setPixmap(createHighDpiDropIndicatorPixmap(size, (Qx::DockWidgetArea)area, m_mode));
}

QPixmap DockOverlayCrossPrivate::createHighDpiDropIndicatorPixmap(const QSizeF &size, Qx::DockWidgetArea dockWidgetArea,
                                                                  DockOverlay::OverlayMode mode)
{
    Q_Q(DockOverlayCross);
    QColor borderColor = iconColor(DockOverlayCross::FrameColor);
    QColor backgroundColor = iconColor(DockOverlayCross::WindowBackgroundColor);
    QColor overlayColor = iconColor(DockOverlayCross::OverlayColor);
    if (overlayColor.alpha() == 255) {
        overlayColor.setAlpha(64);
    }

#if QT_VERSION >= 0x050600
    double devicePixelRatio = q->window()->devicePixelRatioF();
#else
    double devicePixelRatio = q->window()->devicePixelRatio();
#endif
    QSizeF pixmapSize = size * devicePixelRatio;
    QPixmap pm(pixmapSize.toSize());
    pm.fill(QColor(0, 0, 0, 0));

    QPainter p(&pm);
    QPen pen = p.pen();
    QRectF shadowRect(pm.rect());
    QRectF baseRect;
    baseRect.setSize(shadowRect.size() * 0.7);
    baseRect.moveCenter(shadowRect.center());

    // Fill
    QColor shadowColor = iconColor(DockOverlayCross::ShadowColor);
    if (shadowColor.alpha() == 255) {
        shadowColor.setAlpha(64);
    }
    p.fillRect(shadowRect, shadowColor);

    // Drop area rect.
    p.save();
    QRectF areaRect;
    QLineF areaLine;
    QRectF nonAreaRect;
    switch (dockWidgetArea) {
    case Qx::TopDockWidgetArea:
        areaRect = QRectF(baseRect.x(), baseRect.y(), baseRect.width(), baseRect.height() * .5f);
        nonAreaRect = QRectF(baseRect.x(), shadowRect.height() * .5f, baseRect.width(), baseRect.height() * .5f);
        areaLine = QLineF(areaRect.bottomLeft(), areaRect.bottomRight());
        break;
    case Qx::RightDockWidgetArea:
        areaRect = QRectF(shadowRect.width() * .5f, baseRect.y(), baseRect.width() * .5f, baseRect.height());
        nonAreaRect = QRectF(baseRect.x(), baseRect.y(), baseRect.width() * .5f, baseRect.height());
        areaLine = QLineF(areaRect.topLeft(), areaRect.bottomLeft());
        break;
    case Qx::BottomDockWidgetArea:
        areaRect = QRectF(baseRect.x(), shadowRect.height() * .5f, baseRect.width(), baseRect.height() * .5f);
        nonAreaRect = QRectF(baseRect.x(), baseRect.y(), baseRect.width(), baseRect.height() * .5f);
        areaLine = QLineF(areaRect.topLeft(), areaRect.topRight());
        break;
    case Qx::LeftDockWidgetArea:
        areaRect = QRectF(baseRect.x(), baseRect.y(), baseRect.width() * .5f, baseRect.height());
        nonAreaRect = QRectF(shadowRect.width() * .5f, baseRect.y(), baseRect.width() * .5f, baseRect.height());
        areaLine = QLineF(areaRect.topRight(), areaRect.bottomRight());
        break;
    default:
        break;
    }

    QSizeF baseSize = baseRect.size();
    bool isOuterContainerArea =
        (DockOverlay::ContainerOverlayMode == mode) && (dockWidgetArea != Qx::CenterDockWidgetArea);

    if (isOuterContainerArea) {
        baseRect = areaRect;
    }

    p.fillRect(baseRect, backgroundColor);

    if (areaRect.isValid()) {
        pen = p.pen();
        pen.setColor(borderColor);
        p.setBrush(overlayColor);
        p.setPen(Qt::NoPen);
        p.drawRect(areaRect);

        pen = p.pen();
        pen.setWidth(1);
        pen.setColor(borderColor);
        pen.setStyle(Qt::DashLine);
        p.setPen(pen);
        p.drawLine(areaLine);
    }
    p.restore();

    p.save();
    // Draw outer border
    pen = p.pen();
    pen.setColor(borderColor);
    pen.setWidth(1);
    p.setBrush(Qt::NoBrush);
    p.setPen(pen);
    p.drawRect(baseRect);

    // draw window title bar
    p.setBrush(borderColor);
    QRectF frameRect(baseRect.topLeft(), QSizeF(baseRect.width(), baseSize.height() / 10));
    p.drawRect(frameRect);
    p.restore();

    // Draw arrow for outer container drop indicators
    if (isOuterContainerArea) {
        QRectF arrowRect;
        arrowRect.setSize(baseSize);
        arrowRect.setWidth(arrowRect.width() / 4.6);
        arrowRect.setHeight(arrowRect.height() / 2);
        arrowRect.moveCenter(QPointF(0, 0));
        QPolygonF arrow;
        arrow << arrowRect.topLeft() << QPointF(arrowRect.right(), arrowRect.center().y()) << arrowRect.bottomLeft();
        p.setPen(Qt::NoPen);
        p.setBrush(iconColor(DockOverlayCross::ArrowColor));
        p.setRenderHint(QPainter::Antialiasing, true);
        p.translate(nonAreaRect.center().x(), nonAreaRect.center().y());

        switch (dockWidgetArea) {
        case Qx::TopDockWidgetArea:
            p.rotate(-90);
            break;
        case Qx::RightDockWidgetArea:
            break;
        case Qx::BottomDockWidgetArea:
            p.rotate(90);
            break;
        case Qx::LeftDockWidgetArea:
            p.rotate(180);
            break;
        default:
            break;
        }

        p.drawPolygon(arrow);
    }

    pm.setDevicePixelRatio(devicePixelRatio);
    return pm;
}

DockOverlayCross::DockOverlayCross(DockOverlay *overlay)
    : QWidget(overlay->parentWidget())
{
    QX_INIT_PRIVATE(DockOverlayCross);
    Q_D(DockOverlayCross);
    d->m_dockOverlay = overlay;
    d->init();

    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    setWindowTitle("DockOverlayCross");
    setAttribute(Qt::WA_TranslucentBackground);
}

DockOverlayCross::~DockOverlayCross()
{
    QX_FINI_PRIVATE();
}

QColor DockOverlayCross::iconColor(IconColor colorIndex) const
{
    Q_D(const DockOverlayCross);
    return d->m_iconColors[colorIndex];
}

void DockOverlayCross::setIconColor(IconColor colorIndex, const QColor &color)
{
    Q_D(DockOverlayCross);
    d->m_iconColors[colorIndex] = color;
    d->m_updateRequired = true;
}

Qx::DockWidgetArea DockOverlayCross::cursorLocation() const
{
    Q_D(const DockOverlayCross);
    const QPoint pos = mapFromGlobal(QCursor::pos());
    QHashIterator<Qx::DockWidgetArea, QWidget *> i(d->m_dropIndicatorWidgets);
    while (i.hasNext()) {
        i.next();
        if (d->m_dockOverlay->allowedAreas().testFlag(i.key()) && i.value() && i.value()->isVisible() &&
            i.value()->geometry().contains(pos)) {
            return i.key();
        }
    }
    return Qx::InvalidDockWidgetArea;
}

void DockOverlayCross::setupOverlayCross(DockOverlay::OverlayMode mode)
{
    Q_D(DockOverlayCross);
    d->m_mode = mode;

    QHash<Qx::DockWidgetArea, QWidget *> areaWidgets;
    areaWidgets.insert(Qx::TopDockWidgetArea, d->createDropIndicatorWidget(Qx::TopDockWidgetArea, mode));
    areaWidgets.insert(Qx::RightDockWidgetArea, d->createDropIndicatorWidget(Qx::RightDockWidgetArea, mode));
    areaWidgets.insert(Qx::BottomDockWidgetArea, d->createDropIndicatorWidget(Qx::BottomDockWidgetArea, mode));
    areaWidgets.insert(Qx::LeftDockWidgetArea, d->createDropIndicatorWidget(Qx::LeftDockWidgetArea, mode));
    areaWidgets.insert(Qx::CenterDockWidgetArea, d->createDropIndicatorWidget(Qx::CenterDockWidgetArea, mode));

#if QT_VERSION >= 0x050600
    d->m_lastDevicePixelRatio = devicePixelRatioF();
#else
    d->m_lastDevicePixelRatio = devicePixelRatio();
#endif
    setAreaWidgets(areaWidgets);
    d->m_updateRequired = false;
}

void DockOverlayCross::updateOverlayIcons()
{
    Q_D(DockOverlayCross);
    if (windowHandle()->devicePixelRatio() == d->m_lastDevicePixelRatio) {
        return;
    }

    for (auto Widget : d->m_dropIndicatorWidgets) {
        d->updateDropIndicatorIcon(Widget);
    }
#if QT_VERSION >= 0x050600
    d->m_lastDevicePixelRatio = devicePixelRatioF();
#else
    d->m_lastDevicePixelRatio = devicePixelRatio();
#endif
}

void DockOverlayCross::reset()
{
    Q_D(DockOverlayCross);
    QList<Qx::DockWidgetArea> allAreas;
    allAreas << Qx::TopDockWidgetArea << Qx::RightDockWidgetArea << Qx::BottomDockWidgetArea << Qx::LeftDockWidgetArea
             << Qx::CenterDockWidgetArea;
    const Qx::DockWidgetAreas allowedAreas = d->m_dockOverlay->allowedAreas();

    // Update visibility of area widgets based on allowedAreas.
    for (int i = 0; i < allAreas.count(); ++i) {
        QPoint p = d->areaGridPosition(allAreas.at(i));
        QLayoutItem *item = d->m_gridLayout->itemAtPosition(p.x(), p.y());
        QWidget *w = nullptr;
        if (item && (w = item->widget()) != nullptr) {
            w->setVisible(allowedAreas.testFlag(allAreas.at(i)));
        }
    }
}

void DockOverlayCross::updatePosition()
{
    Q_D(DockOverlayCross);
    resize(d->m_dockOverlay->size());
    QPoint topLeft = d->m_dockOverlay->pos();
    QPoint offest((this->width() - d->m_dockOverlay->width()) / 2, (this->height() - d->m_dockOverlay->height()) / 2);
    QPoint crossTopLeft = topLeft - offest;
    move(crossTopLeft);
}

void DockOverlayCross::setIconColors(const QString &colors)
{
    Q_D(DockOverlayCross);
    static const QMap<QString, int> ColorCompenentStringMap{{"Frame", DockOverlayCross::FrameColor},
                                                            {"Background", DockOverlayCross::WindowBackgroundColor},
                                                            {"Overlay", DockOverlayCross::OverlayColor},
                                                            {"Arrow", DockOverlayCross::ArrowColor},
                                                            {"Shadow", DockOverlayCross::ShadowColor}};

#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
    auto skipEmptyParts = QString::SkipEmptyParts;
#else
    auto skipEmptyParts = Qt::SkipEmptyParts;
#endif
    auto colorList = colors.split(' ', skipEmptyParts);
    for (const auto &colorListEntry : colorList) {
        auto componentColor = colorListEntry.split('=', skipEmptyParts);
        int component = ColorCompenentStringMap.value(componentColor[0], -1);
        if (component < 0) {
            continue;
        }
        d->m_iconColors[component] = QColor(componentColor[1]);
    }

    d->m_updateRequired = true;
}

QString DockOverlayCross::iconColors() const
{
    return QString();
}

void DockOverlayCross::setAreaWidgets(const QHash<Qx::DockWidgetArea, QWidget *> &widgets)
{
    Q_D(DockOverlayCross);
    // Delete old widgets.
    QMutableHashIterator<Qx::DockWidgetArea, QWidget *> i(d->m_dropIndicatorWidgets);
    while (i.hasNext()) {
        i.next();
        QWidget *widget = i.value();
        d->m_gridLayout->removeWidget(widget);
        delete widget;
        i.remove();
    }

    // Insert new widgets into grid.
    d->m_dropIndicatorWidgets = widgets;
    QHashIterator<Qx::DockWidgetArea, QWidget *> i2(d->m_dropIndicatorWidgets);
    while (i2.hasNext()) {
        i2.next();
        const Qx::DockWidgetArea area = i2.key();
        QWidget *widget = i2.value();
        QPoint p = d->areaGridPosition(area);
        d->m_gridLayout->addWidget(widget, p.x(), p.y(), (Qt::Alignment)areaAlignment(area));
    }

    if (DockOverlay::PanelOverlayMode == d->m_mode) {
        d->m_gridLayout->setContentsMargins(0, 0, 0, 0);
        d->m_gridLayout->setRowStretch(0, 1);
        d->m_gridLayout->setRowStretch(1, 0);
        d->m_gridLayout->setRowStretch(2, 0);
        d->m_gridLayout->setRowStretch(3, 0);
        d->m_gridLayout->setRowStretch(4, 1);

        d->m_gridLayout->setColumnStretch(0, 1);
        d->m_gridLayout->setColumnStretch(1, 0);
        d->m_gridLayout->setColumnStretch(2, 0);
        d->m_gridLayout->setColumnStretch(3, 0);
        d->m_gridLayout->setColumnStretch(4, 1);
    } else {
        d->m_gridLayout->setContentsMargins(4, 4, 4, 4);
        d->m_gridLayout->setRowStretch(0, 0);
        d->m_gridLayout->setRowStretch(1, 1);
        d->m_gridLayout->setRowStretch(2, 1);
        d->m_gridLayout->setRowStretch(3, 1);
        d->m_gridLayout->setRowStretch(4, 0);

        d->m_gridLayout->setColumnStretch(0, 0);
        d->m_gridLayout->setColumnStretch(1, 1);
        d->m_gridLayout->setColumnStretch(2, 1);
        d->m_gridLayout->setColumnStretch(3, 1);
        d->m_gridLayout->setColumnStretch(4, 0);
    }
    reset();
}

void DockOverlayCross::showEvent(QShowEvent *e)
{
    Q_UNUSED(e);
    Q_D(DockOverlayCross);
    if (d->m_updateRequired) {
        setupOverlayCross(d->m_mode);
    }
    this->updatePosition();
}

QX_DOCK_END_NAMESPACE
