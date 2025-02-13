/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "dockautohidecontainer.h"
#include "dockwidget.h"
#include "dockcontainer.h"
#include "docksidetab.h"
#include "dockpanel.h"
#include "dockwindow.h"
#include "dockresizehandle.h"
#include "dockmanager.h"
#include "dockutils.h"
#include "dockfloatingcontainer.h"
#include "docksidebar.h"

#include <QBoxLayout>
#include <QApplication>
#include <QXmlStreamWriter>
#include <QPointer>

QX_DOCK_BEGIN_NAMESPACE

static const int s_resizeMargin = 30;

static bool isHorizontalArea(Qx::DockSideBarArea area)
{
    switch (area) {
    case Qx::DockSideBarTop:
    case Qx::DockSideBarBottom:
        return true;
    case Qx::DockSideBarLeft:
    case Qx::DockSideBarRight:
        return false;
    default:
        return true;
    }

    return true;
}
static Qt::Edge edgeFromSideBarArea(Qx::DockSideBarArea area)
{
    switch (area) {
    case Qx::DockSideBarTop:
        return Qt::BottomEdge;
    case Qx::DockSideBarBottom:
        return Qt::TopEdge;
    case Qx::DockSideBarLeft:
        return Qt::RightEdge;
    case Qx::DockSideBarRight:
        return Qt::LeftEdge;
    default:
        return Qt::LeftEdge;
    }

    return Qt::LeftEdge;
}

int resizeHandleLayoutPosition(Qx::DockSideBarArea area)
{
    switch (area) {
    case Qx::DockSideBarBottom:
    case Qx::DockSideBarRight:
        return 0;

    case Qx::DockSideBarTop:
    case Qx::DockSideBarLeft:
        return 1;

    default:
        return 0;
    }

    return 0;
}

/**
 * Returns true if the object given in ancestor is an ancestor of the object
 * given in descendant
 */
static bool objectIsAncestorOf(const QObject *descendant, const QObject *ancestor)
{
    if (!ancestor) {
        return false;
    }
    while (descendant) {
        if (descendant == ancestor) {
            return true;
        }
        descendant = descendant->parent();
    }
    return false;
}

/**
 * Returns true if the object given in ancestor is the object given in descendant
 * or if it is an ancestor of the object given in descendant
 */
static bool isObjectOrAncestor(const QObject *descendant, const QObject *ancestor)
{
    if (ancestor && (descendant == ancestor)) {
        return true;
    } else {
        return objectIsAncestorOf(descendant, ancestor);
    }
}

class DockAutoHideContainerPrivate
{
public:
    QX_DECLARE_PUBLIC(DockAutoHideContainer)
public:
    DockAutoHideContainerPrivate();

    bool isHorizontal() const;

    Qx::DockWidgetArea getDockWidgetArea(Qx::DockSideBarArea area);

    void updateResizeHandleSizeLimitMax();
    void forwardEventToDockContainer(QEvent *event);
public:
    DockPanel *m_panel = nullptr;
    DockWidget *m_dockWidget = nullptr;
    Qx::DockSideBarArea m_sideTabBarArea = Qx::DockSideBarNone;
    QBoxLayout *m_layout = nullptr;
    DockResizeHandle *m_resizeHandle = nullptr;
    QPointer<DockSideTab> m_sideTab;
    QSize m_size;   // creates invalid size
    QSize m_sizeCache;
};

DockAutoHideContainerPrivate::DockAutoHideContainerPrivate()
{
}

bool DockAutoHideContainerPrivate::isHorizontal() const
{
    return isHorizontalArea(m_sideTabBarArea);
}

Qx::DockWidgetArea DockAutoHideContainerPrivate::getDockWidgetArea(Qx::DockSideBarArea area)
{
    switch (area) {
    case Qx::DockSideBarLeft:
        return Qx::LeftDockWidgetArea;
    case Qx::DockSideBarRight:
        return Qx::RightDockWidgetArea;
    case Qx::DockSideBarBottom:
        return Qx::BottomDockWidgetArea;
    case Qx::DockSideBarTop:
        return Qx::TopDockWidgetArea;
    default:
        return Qx::LeftDockWidgetArea;
    }

    return Qx::LeftDockWidgetArea;
}

void DockAutoHideContainerPrivate::updateResizeHandleSizeLimitMax()
{
    Q_Q(DockAutoHideContainer);
    auto rect = q->dockContainer()->contentRect();
    const auto maxResizeHandleSize = m_resizeHandle->orientation() == Qt::Horizontal ? rect.width() : rect.height();
    m_resizeHandle->setMaxResizeSize(maxResizeHandleSize - s_resizeMargin);
}

void DockAutoHideContainerPrivate::forwardEventToDockContainer(QEvent *event)
{
    Q_Q(DockAutoHideContainer);
    auto container = q->dockContainer();
    if (container) {
        container->handleAutoHideWidgetEvent(event, q);
    }
}

DockAutoHideContainer::DockAutoHideContainer(DockWidget *w, Qx::DockSideBarArea area, DockContainer *parent)
    : Super(parent)
{
    QX_INIT_PRIVATE(DockAutoHideContainer);
    Q_D(DockAutoHideContainer);

    hide();   // auto hide dock container is initially always hidden
    d->m_sideTabBarArea = area;
    d->m_sideTab = new DockSideTab();
    connect(d->m_sideTab, &DockSideTab::pressed, this, &DockAutoHideContainer::toggleCollapseState);
    d->m_panel = new DockPanel(w->dockWindow(), parent);
    d->m_panel->setObjectName("autoHideDockPanel");
    d->m_panel->setAutoHideContainer(this);

    setObjectName("autoHideDockContainer");

    d->m_layout = new QBoxLayout(isHorizontalArea(area) ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight);
    d->m_layout->setContentsMargins(0, 0, 0, 0);
    d->m_layout->setSpacing(0);
    setLayout(d->m_layout);

    d->m_resizeHandle = new DockResizeHandle(edgeFromSideBarArea(area), this);
    d->m_resizeHandle->setMinResizeSize(64);
    bool opaqueResize = DockManager::testConfigFlag(DockManager::OpaqueSplitterResize);
    d->m_resizeHandle->setOpaqueResize(opaqueResize);

    d->m_size = d->m_panel->size();
    d->m_sizeCache = w->size();

    addDockWidget(w);
    parent->registerAutoHideWidget(this);
    // The dock area should not be added to the layout before it contains the
    // dock widget. If you add it to the layout before it contains the dock widget
    // then you will likely see this warning for OpenGL widgets or QAxWidgets:
    // setGeometry: Unable to set geometry XxY+Width+Height on QWidgetWindow/'WidgetClassWindow
    d->m_layout->addWidget(d->m_panel);
    d->m_layout->insertWidget(resizeHandleLayoutPosition(area), d->m_resizeHandle);
}

DockAutoHideContainer::~DockAutoHideContainer()
{
    Q_D(DockAutoHideContainer);
    // Remove event filter in case there are any queued messages
    qApp->removeEventFilter(this);
    if (dockContainer()) {
        dockContainer()->removeAutoHideWidget(this);
    }

    if (d->m_sideTab) {
        delete d->m_sideTab;
    }
    QX_FINI_PRIVATE();
}

DockSideBar *DockAutoHideContainer::autoHideSideBar() const
{
    Q_D(const DockAutoHideContainer);
    if (d->m_sideTab) {
        return d->m_sideTab->sideBar();
    } else {
        auto container = this->dockContainer();
        return container ? container->autoHideSideBar(d->m_sideTabBarArea) : nullptr;
    }
}

DockSideTab *DockAutoHideContainer::autoHideTab() const
{
    Q_D(const DockAutoHideContainer);
    return d->m_sideTab;
}

/**
 * Returns the index of this container in the sidebar
 */
int DockAutoHideContainer::tabIndex() const
{
    Q_D(const DockAutoHideContainer);
    return d->m_sideTab->tabIndex();
}

DockWidget *DockAutoHideContainer::dockWidget() const
{
    Q_D(const DockAutoHideContainer);
    return d->m_dockWidget;
}

void DockAutoHideContainer::addDockWidget(DockWidget *w)
{
    Q_D(DockAutoHideContainer);
    if (d->m_dockWidget) {
        // Remove the old dock widget at this area
        d->m_panel->removeDockWidget(d->m_dockWidget);
    }

    d->m_dockWidget = w;
    d->m_sideTab->setDockWidget(w);
    DockPanel *oldPanel = w->dockPanel();
    auto isRestoringState = w->dockWindow()->isRestoringState();
    if (oldPanel && !isRestoringState) {
        // The initial size should be a little bit bigger than the original dock
        // area size to prevent that the resize handle of this auto hid dock area
        // is near of the splitter of the old dock area.
        d->m_size = oldPanel->size() + QSize(16, 16);
        oldPanel->removeDockWidget(w);
    }
    d->m_panel->addDockWidget(w);
    updateSize();
    // The dock area is not visible and will not update the size when updateSize()
    // is called for this auto hide container. Therefore we explicitly resize
    // it here. As soon as it will become visible, it will get the right size
    d->m_panel->resize(size());
}

DockContainer *DockAutoHideContainer::dockContainer() const
{
    return internal::findParent<DockContainer *>(this);
}

Qx::DockSideBarArea DockAutoHideContainer::sideBarArea() const
{
    Q_D(const DockAutoHideContainer);
    return d->m_sideTabBarArea;
}

/**
 * Sets a new DockSideBarArea.
 * If a new side bar area is set, the auto hide dock container needs
 * to update its resize handle position
 */
void DockAutoHideContainer::setSideBarArea(Qx::DockSideBarArea area)
{
    Q_D(DockAutoHideContainer);
    if (d->m_sideTabBarArea == area) {
        return;
    }

    d->m_sideTabBarArea = area;
    d->m_layout->removeWidget(d->m_resizeHandle);
    d->m_layout->setDirection(isHorizontalArea(area) ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight);
    d->m_layout->insertWidget(resizeHandleLayoutPosition(area), d->m_resizeHandle);
    d->m_resizeHandle->setHandlePosition(edgeFromSideBarArea(area));
    internal::repolishStyle(this, internal::RepolishDirectChildren);
}

/**
 * Cleanups up the side tab widget and then deletes itself
 */
void DockAutoHideContainer::cleanupAndDelete()
{
    Q_D(DockAutoHideContainer);
    const auto dockWidget = d->m_dockWidget;
    if (dockWidget) {
        auto sideTab = d->m_sideTab;
        sideTab->removeFromSideBar();
        sideTab->setParent(nullptr);
        sideTab->hide();
    }

    hide();
    deleteLater();
}

/**
 * Moves the contents to the parent container widget
 * Used before removing this Auto Hide dock container
 */
void DockAutoHideContainer::moveContentsToParent()
{
    Q_D(DockAutoHideContainer);
    cleanupAndDelete();
    // If we unpin the auto hide dock widget, then we insert it into the same
    // location like it had as a auto hide widget.  This brings the least surprise
    // to the user and he does not have to search where the widget was inserted.
    d->m_dockWidget->setDockPanel(nullptr);
    auto container = this->dockContainer();
    container->addDockWidget(d->getDockWidgetArea(d->m_sideTabBarArea), d->m_dockWidget);
}

/**
 * Removes the AutoHide container from the current side bar and adds
 * it to the new side bar given in area
 */
void DockAutoHideContainer::moveToNewSideBarArea(Qx::DockSideBarArea area, int tabIndex)
{
    if (area == sideBarArea() && tabIndex == this->tabIndex()) {
        return;
    }

    auto oldOrientation = orientation();
    auto sideBar = dockContainer()->autoHideSideBar(area);
    sideBar->addAutoHideWidget(this, tabIndex);
    // If we move a horizontal auto hide container to a vertical position
    // then we resize it to the original dock widget size, to avoid
    // an extremely stretched dock widget after insertion
    if (sideBar->orientation() != oldOrientation) {
        resetToInitialDockWidgetSize();
    }
}

/*
 * Toggles the auto Hide dock container widget
 * This will also hide the side tab widget
 */
void DockAutoHideContainer::toggleView(bool enable)
{
    Q_D(DockAutoHideContainer);
    if (enable) {
        if (d->m_sideTab) {
            d->m_sideTab->show();
        }
    } else {
        if (d->m_sideTab) {
            d->m_sideTab->hide();
        }
        hide();
        qApp->removeEventFilter(this);
    }
}

/*
 * Collapses the auto hide dock container widget
 * Does not hide the side tab widget
 */
void DockAutoHideContainer::collapseView(bool enable)
{
    Q_D(DockAutoHideContainer);
    if (enable) {
        hide();
        qApp->removeEventFilter(this);
    } else {
        updateSize();
        d->updateResizeHandleSizeLimitMax();
        raise();
        show();
        d->m_dockWidget->dockWindow()->setDockWidgetFocused(d->m_dockWidget);
        qApp->installEventFilter(this);
    }

    d->m_sideTab->updateStyle();
}

/**
 * Toggles the current collapse state
 */
void DockAutoHideContainer::toggleCollapseState()
{
    collapseView(isVisible());
}

/**
 * Use this instead of resize.
 * Depending on the sidebar area this will set the width or height
 * of this auto hide container.
 */
void DockAutoHideContainer::setSize(int size)
{
    Q_D(DockAutoHideContainer);
    if (d->isHorizontal()) {
        d->m_size.setHeight(size);
    } else {
        d->m_size.setWidth(size);
    }

    updateSize();
}

/**
 * Resets the width or height to the initial dock widget size dependinng on
 * the orientation.
 * If the orientation is Qt::Horizontal, then the height is reset to
 * the initial size and if orientation is Qt::Vertical, then the width is
 * reset to the initial size
 */
void DockAutoHideContainer::resetToInitialDockWidgetSize()
{
    Q_D(DockAutoHideContainer);
    if (orientation() == Qt::Horizontal) {
        setSize(d->m_sizeCache.height());
    } else {
        setSize(d->m_sizeCache.width());
    }
}

/**
 * Returns orientation of this container.
 * Left and right containers have a Qt::Vertical orientation and top / bottom
 * containers have a Qt::Horizontal orientation.
 * The function returns the orientation of the corresponding auto hide
 * side bar.
 */
Qt::Orientation DockAutoHideContainer::orientation() const
{
    Q_D(const DockAutoHideContainer);
    return internal::isHorizontalSideBarArea(d->m_sideTabBarArea) ? Qt::Horizontal : Qt::Vertical;
}

bool DockAutoHideContainer::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(DockAutoHideContainer);
    // A switch case statement would be nicer here, but we cannot use
    // internal::FloatingWidgetDragStartEvent in a switch case
    if (event->type() == QEvent::Resize) {
        if (!d->m_resizeHandle->isResizing()) {
            updateSize();
        }
    } else if (event->type() == QEvent::MouseButtonPress) {
        auto widget = qobject_cast<QWidget *>(watched);
        // Ignore non widget events
        if (!widget) {
            return Super::eventFilter(watched, event);
        }

        // Now check, if the user clicked into the side tab and ignore this event,
        // because the side tab click handler will call collapseView(). If we
        // do not ignore this here, then we will collapse the container and the side tab
        // click handler will uncollapse it
        if (widget == d->m_sideTab.data()) {
            return Super::eventFilter(watched, event);
        }

        // Now we check, if the user clicked inside of this auto hide container.
        // If the click is inside of this auto hide container, then we can
        // ignore the event, because the auto hide overlay should not get collapsed if
        // user works in it
        if (isObjectOrAncestor(widget, this)) {
            return Super::eventFilter(watched, event);
        }

        // Ignore the mouse click if it is not inside of this container
        if (!isObjectOrAncestor(widget, dockContainer())) {
            return Super::eventFilter(watched, event);
        }

        // user clicked into container - collapse the auto hide widget
        collapseView(true);
    } else if (event->type() == internal::FloatingWidgetDragStartEvent) {
        // If we are dragging our own floating widget, the we do not need to
        // collapse the view
        auto floatingWidget = dockContainer()->floatingWidget();
        if (floatingWidget != watched) {
            collapseView(true);
        }
    } else if (event->type() == internal::DockedWidgetDragStartEvent) {
        collapseView(true);
    }

    return Super::eventFilter(watched, event);
}

void DockAutoHideContainer::resizeEvent(QResizeEvent *event)
{
    Q_D(DockAutoHideContainer);
    Super::resizeEvent(event);
    if (d->m_resizeHandle->isResizing()) {
        d->m_size = this->size();
        d->updateResizeHandleSizeLimitMax();
    }
}

void DockAutoHideContainer::leaveEvent(QEvent *event)
{
    Q_D(DockAutoHideContainer);
    // Resizing of the dock container via the resize handle in non opaque mode
    // mays cause a leave event that is not really a leave event. Therefore
    // we check here, if we are really outside of our rect.
    auto pos = mapFromGlobal(QCursor::pos());
    if (!rect().contains(pos)) {
        d->forwardEventToDockContainer(event);
    }
    Super::leaveEvent(event);
}

bool DockAutoHideContainer::event(QEvent *event)
{
    Q_D(DockAutoHideContainer);
    switch (event->type()) {
    case QEvent::Enter:
    case QEvent::Hide:
        d->forwardEventToDockContainer(event);
        break;

    case QEvent::MouseButtonPress:
        return true;
        break;

    default:
        break;
    }

    return Super::event(event);
}

/**
 * Updates the size considering the size limits and the resize margins
 */
void DockAutoHideContainer::updateSize()
{
    Q_D(DockAutoHideContainer);
    auto dockContainerParent = dockContainer();
    if (!dockContainerParent) {
        return;
    }

    auto rect = dockContainerParent->contentRect();
    switch (sideBarArea()) {
    case Qx::DockSideBarTop:
        resize(rect.width(), qMin(rect.height() - s_resizeMargin, d->m_size.height()));
        move(rect.topLeft());
        break;

    case Qx::DockSideBarLeft:
        resize(qMin(d->m_size.width(), rect.width() - s_resizeMargin), rect.height());
        move(rect.topLeft());
        break;

    case Qx::DockSideBarRight: {
        resize(qMin(d->m_size.width(), rect.width() - s_resizeMargin), rect.height());
        QPoint p = rect.topRight();
        p.rx() -= (width() - 1);
        move(p);
    } break;

    case Qx::DockSideBarBottom: {
        resize(rect.width(), qMin(rect.height() - s_resizeMargin, d->m_size.height()));
        QPoint p = rect.bottomLeft();
        p.ry() -= (height() - 1);
        move(p);
    } break;

    default:
        break;
    }

    if (orientation() == Qt::Horizontal) {
        d->m_sizeCache.setHeight(this->height());
    } else {
        d->m_sizeCache.setWidth(this->width());
    }
}

void DockAutoHideContainer::saveState(QXmlStreamWriter &s) const
{
    Q_D(const DockAutoHideContainer);
    s.writeStartElement("Widget");
    s.writeAttribute("Name", d->m_dockWidget->objectName());
    s.writeAttribute("Closed", QString::number(d->m_dockWidget->isClosed() ? 1 : 0));
    s.writeAttribute("Size", QString::number(d->isHorizontal() ? d->m_size.height() : d->m_size.width()));
    s.writeEndElement();
}

QX_DOCK_END_NAMESPACE
