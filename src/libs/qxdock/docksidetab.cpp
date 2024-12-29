/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "docksidetab.h"
#include "docksidebar.h"
#include "dockmanager.h"
#include "dockwidget.h"
#include "dockutils.h"
#include "dockcontainer.h"
#include "dockfloatingcontainer.h"
#include "dockfloatingpreview.h"
#include "dockpanel.h"
#include "dockoverlay.h"
#include "dockautohidecontainer.h"
#include "dockwindow.h"

#include <QElapsedTimer>
#include <QApplication>
#include <QMenu>

QX_DOCK_BEGIN_NAMESPACE

class DockSideTabPrivate
{
public:
    QX_DECLARE_PUBLIC(DockSideTab)
public:
    DockSideTabPrivate();
    void updateOrientation();

    DockContainer *dockContainer() const;
    void forwardEventToDockContainer(QEvent *event);
    QAction *createAutoHideToAction(const QString &title, Qx::DockSideBarArea area, QMenu *menu);
    void saveDragStartMousePosition(const QPoint &globalPos);
    bool isDraggingState(Qx::DockDragState dragState) const;
    bool startFloating(Qx::DockDragState draggingState = Qx::DockDraggingFloatingWidget);

    template <typename T> DockFloatingWidget *createFloatingWidget(T *widget)
    {
        Q_Q(DockSideTab);
        auto w = new DockFloatingPreview(widget);
        q->connect(w, &DockFloatingPreview::draggingCanceled, [=]() {
            m_dragState = Qx::DockDraggingInactive;
        });
        return w;
    }
public:
    DockSideBar *m_sideBar = nullptr;
    DockWidget *m_dockWidget = nullptr;
    Qt::Orientation m_orientation{Qt::Vertical};
    QElapsedTimer m_timeSinceHoverMousePress;
    bool m_mousePressed = false;
    Qx::DockDragState m_dragState = Qx::DockDraggingInactive;
    QPoint m_globalDragStartMousePosition;
    QPoint m_dragStartMousePosition;
    DockFloatingWidget *m_floatingWidget = nullptr;
    Qt::Orientation m_dragStartOrientation;
};

DockSideTabPrivate::DockSideTabPrivate()
{
}

void DockSideTabPrivate::updateOrientation()
{
    Q_Q(DockSideTab);
    bool IconOnly = DockManager::testAutoHideConfigFlag(DockManager::AutoHideSideBarsIconOnly);
    if (IconOnly && !q->icon().isNull()) {
        q->setText("");
        q->setOrientation(Qt::Horizontal);
    } else {
        auto area = m_sideBar->sideBarArea();
        q->setOrientation((area == Qx::DockSideBarBottom || area == Qx::DockSideBarTop) ? Qt::Horizontal
                                                                                        : Qt::Vertical);
    }
}

DockContainer *DockSideTabPrivate::dockContainer() const
{
    return m_dockWidget ? m_dockWidget->dockContainer() : nullptr;
}

void DockSideTabPrivate::forwardEventToDockContainer(QEvent *event)
{
    Q_Q(DockSideTab);
    auto container = dockContainer();
    if (container) {
        container->handleAutoHideWidgetEvent(event, q);
    }
}

QAction *DockSideTabPrivate::createAutoHideToAction(const QString &title, Qx::DockSideBarArea area, QMenu *menu)
{
    Q_Q(DockSideTab);
    auto action = menu->addAction(title);
    action->setProperty(internal::LocationProperty, area);
    QObject::connect(action, &QAction::triggered, q, &DockSideTab::onAutoHideToActionClicked);
    action->setEnabled(area != q->sideBarArea());
    return action;
}

void DockSideTabPrivate::saveDragStartMousePosition(const QPoint &globalPos)
{
    Q_Q(DockSideTab);
    m_globalDragStartMousePosition = globalPos;
    m_dragStartMousePosition = q->mapFromGlobal(globalPos);
}

bool DockSideTabPrivate::isDraggingState(Qx::DockDragState dragState) const
{
    return this->m_dragState == dragState;
}

bool DockSideTabPrivate::startFloating(Qx::DockDragState draggingState)
{
    Q_Q(DockSideTab);
    auto panel = m_dockWidget->dockPanel();
    m_dragState = draggingState;
    DockFloatingWidget *floatingWidget = nullptr;
    floatingWidget = createFloatingWidget(panel);
    auto size = panel->size();
    auto startPos = m_dragStartMousePosition;
    auto autoHideContainer = m_dockWidget->autoHideContainer();
    m_dragStartOrientation = autoHideContainer->orientation();
    switch (m_sideBar->sideBarArea()) {
    case Qx::DockSideBarLeft:
        startPos.rx() = autoHideContainer->rect().left() + 10;
        break;

    case Qx::DockSideBarRight:
        startPos.rx() = autoHideContainer->rect().right() - 10;
        break;

    case Qx::DockSideBarTop:
        startPos.ry() = autoHideContainer->rect().top() + 10;
        break;

    case Qx::DockSideBarBottom:
        startPos.ry() = autoHideContainer->rect().bottom() - 10;
        break;

    case Qx::DockSideBarNone:
        return false;
    }
    floatingWidget->startFloating(startPos, size, Qx::DockDraggingFloatingWidget, q);
    auto dockWindow = m_dockWidget->dockWindow();
    auto Overlay = dockWindow->containerOverlay();
    Overlay->setAllowedAreas(Qx::OuterDockAreas);
    this->m_floatingWidget = floatingWidget;
    qApp->postEvent(m_dockWidget, new QEvent((QEvent::Type)internal::DockedWidgetDragStartEvent));

    return true;
}

DockSideTab::DockSideTab(QWidget *parent)
    : DockButton(parent)
{
    QX_INIT_PRIVATE(DockSideTab);
    setAttribute(Qt::WA_NoMousePropagation);
    setFocusPolicy(Qt::NoFocus);
}

DockSideTab::~DockSideTab()
{
    QX_FINI_PRIVATE();
}

DockWidget *DockSideTab::dockWidget() const
{
    Q_D(const DockSideTab);
    return d->m_dockWidget;
}

void DockSideTab::setDockWidget(DockWidget *w)
{
    Q_D(DockSideTab);
    if (!w) {
        return;
    }
    d->m_dockWidget = w;
    setText(w->windowTitle());
    setIcon(d->m_dockWidget->icon());
    setToolTip(w->windowTitle());
}

Qt::Orientation DockSideTab::orientation() const
{
    Q_D(const DockSideTab);
    return d->m_orientation;
}

/**
 * Set orientation vertical or horizontal
 */
void DockSideTab::setOrientation(Qt::Orientation orientation)
{
    Q_D(DockSideTab);
    d->m_orientation = orientation;
    if (orientation == Qt::Horizontal) {
        setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    } else {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    }
    DockButton::setButtonOrientation((Qt::Horizontal == orientation) ? DockButton::Horizontal
                                                                     : DockButton::VerticalTopToBottom);
    updateStyle();
}

void DockSideTab::updateStyle()
{
    internal::repolishStyle(this, internal::RepolishDirectChildren);
    update();
}

Qx::DockSideBarArea DockSideTab::sideBarArea() const
{
    Q_D(const DockSideTab);
    if (d->m_sideBar) {
        return d->m_sideBar->sideBarArea();
    }

    return Qx::DockSideBarLeft;
}

/**
 * Returns true, if this is the active tab. The tab is active if the auto
 * hide widget is visible
 */
bool DockSideTab::isActiveTab() const
{
    Q_D(const DockSideTab);
    if (d->m_dockWidget && d->m_dockWidget->autoHideContainer()) {
        return d->m_dockWidget->autoHideContainer()->isVisible();
    }

    return false;
}

/**
 * Returns true if the auto hide config flag AutoHideSideBarsIconOnly
 * is set and if the tab has an icon - that means the icon is not null
 */
bool DockSideTab::iconOnly() const
{
    return DockManager::testAutoHideConfigFlag(DockManager::AutoHideSideBarsIconOnly) && !icon().isNull();
}

/**
 * Returns the side bar that contains this tab or a nullptr if the tab is
 * not in a side bar
 */
DockSideBar *DockSideTab::sideBar() const
{
    Q_D(const DockSideTab);
    return d->m_sideBar;
}

/**
 * Returns the index of this tab in the sideBar
 */
int DockSideTab::tabIndex() const
{
    Q_D(const DockSideTab);
    if (!d->m_sideBar) {
        return -1;
    }

    return d->m_sideBar->indexOfTab(*this);
}

void DockSideTab::setDockWidgetFloating()
{
    Q_D(DockSideTab);
    d->m_dockWidget->setFloating();
}

void DockSideTab::unpinDockWidget()
{
    Q_D(DockSideTab);
    d->m_dockWidget->setAutoHide(false);
}

void DockSideTab::requestCloseDockWidget()
{
    Q_D(DockSideTab);
    d->m_dockWidget->requestCloseDockWidget();
}

void DockSideTab::onAutoHideToActionClicked()
{
    Q_D(DockSideTab);
    int location = sender()->property(internal::LocationProperty).toInt();
    d->m_dockWidget->setAutoHide(true, (Qx::DockSideBarArea)location);
}

void DockSideTab::setSideBar(DockSideBar *sideBar)
{
    Q_D(DockSideTab);
    d->m_sideBar = sideBar;
    if (d->m_sideBar) {
        d->updateOrientation();
    }
}

void DockSideTab::removeFromSideBar()
{
    Q_D(DockSideTab);
    if (d->m_sideBar == nullptr) {
        return;
    }
    d->m_sideBar->removeTab(this);
    setSideBar(nullptr);
}

bool DockSideTab::event(QEvent *e)
{
    Q_D(DockSideTab);
    if (!DockManager::testAutoHideConfigFlag(DockManager::AutoHideShowOnMouseOver)) {
        return Super::event(e);
    }

    switch (e->type()) {
    case QEvent::Enter:
    case QEvent::Leave:
        d->forwardEventToDockContainer(e);
        break;

    default:
        break;
    }
    return Super::event(e);
}

void DockSideTab::mousePressEvent(QMouseEvent *e)
{
    Q_D(DockSideTab);
    // If AutoHideShowOnMouseOver is active, then the showing is triggered
    // by a MousePressEvent sent to this tab. To prevent accidental hiding
    // of the tab by a mouse click, we wait at least 500 ms before we accept
    // the mouse click
    if (!e->spontaneous()) {
        d->m_timeSinceHoverMousePress.restart();
        d->forwardEventToDockContainer(e);
    } else if (d->m_timeSinceHoverMousePress.hasExpired(500)) {
        d->forwardEventToDockContainer(e);
    }

    if (e->button() == Qt::LeftButton) {
        e->accept();
        d->m_mousePressed = true;
        d->saveDragStartMousePosition(internal::globalPositionOf(e));
        d->m_dragState = Qx::DockDraggingMousePressed;
    }
    Super::mousePressEvent(e);
}

void DockSideTab::mouseReleaseEvent(QMouseEvent *e)
{
    Q_D(DockSideTab);
    if (e->button() == Qt::LeftButton) {
        d->m_mousePressed = false;
        auto currentDragState = d->m_dragState;
        d->m_globalDragStartMousePosition = QPoint();
        d->m_dragStartMousePosition = QPoint();
        d->m_dragState = Qx::DockDraggingInactive;

        switch (currentDragState) {
        case Qx::DockDraggingTab:
            break;

        case Qx::DockDraggingFloatingWidget:
            e->accept();
            d->m_floatingWidget->finishDragging();
            if (d->m_dockWidget->autoHideContainer() && d->m_dragStartOrientation != orientation()) {
                d->m_dockWidget->autoHideContainer()->resetToInitialDockWidgetSize();
            }
            break;

        default:
            break;   // do nothing
        }
    }

    Super::mouseReleaseEvent(e);
}

void DockSideTab::mouseMoveEvent(QMouseEvent *e)
{
    Q_D(DockSideTab);
    if (!(e->buttons() & Qt::LeftButton) || d->isDraggingState(Qx::DockDraggingInactive)) {
        d->m_dragState = Qx::DockDraggingInactive;
        Super::mouseMoveEvent(e);
        return;
    }

    // move floating window
    if (d->isDraggingState(Qx::DockDraggingFloatingWidget)) {
        d->m_floatingWidget->moveFloating();
        Super::mouseMoveEvent(e);
        return;
    }

    // move tab
    if (d->isDraggingState(Qx::DockDraggingTab)) {
        // Moving the tab is always allowed because it does not mean moving the
        // dock widget around
        // d->moveTab(ev);
    }

    auto mappedPos = mapToParent(e->pos());
    bool mouseOutsideBar = (mappedPos.x() < 0) || (mappedPos.x() > parentWidget()->rect().right());
    // Maybe a fixed drag distance is better here ?
    int dragDistanceY = qAbs(d->m_globalDragStartMousePosition.y() - internal::globalPositionOf(e).y());
    if (dragDistanceY >= DockManager::startDragDistance() || mouseOutsideBar) {
        // Floating is only allowed for widgets that are floatable
        // We can create the drag preview if the widget is movable.
        auto Features = d->m_dockWidget->features();
        if (Features.testFlag(DockWidget::DockWidgetFloatable) || (Features.testFlag(DockWidget::DockWidgetMovable))) {
            d->startFloating();
        }
        return;
    }

    Super::mouseMoveEvent(e);
}

void DockSideTab::contextMenuEvent(QContextMenuEvent *e)
{
    Q_D(DockSideTab);
    e->accept();
    d->saveDragStartMousePosition(e->globalPos());

    const bool isFloatable = d->m_dockWidget->features().testFlag(DockWidget::DockWidgetFloatable);
    QAction *action;
    QMenu menu(this);

    action = menu.addAction(tr("Detach"), this, SLOT(setDockWidgetFloating()));
    action->setEnabled(isFloatable);
    auto IsPinnable = d->m_dockWidget->features().testFlag(DockWidget::DockWidgetPinnable);
    action->setEnabled(IsPinnable);

    auto pinMenu = menu.addMenu(tr("Pin To..."));
    pinMenu->setEnabled(IsPinnable);
    d->createAutoHideToAction(tr("Top"), Qx::DockSideBarTop, pinMenu);
    d->createAutoHideToAction(tr("Left"), Qx::DockSideBarLeft, pinMenu);
    d->createAutoHideToAction(tr("Right"), Qx::DockSideBarRight, pinMenu);
    d->createAutoHideToAction(tr("Bottom"), Qx::DockSideBarBottom, pinMenu);

    action = menu.addAction(tr("Unpin (Dock)"), this, SLOT(unpinDockWidget()));
    menu.addSeparator();
    action = menu.addAction(tr("Close"), this, SLOT(requestCloseDockWidget()));
    action->setEnabled(d->m_dockWidget->features().testFlag(DockWidget::DockWidgetClosable));

    menu.exec(e->globalPos());
}

QX_DOCK_END_NAMESPACE
