/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockwindow.h"
#include "dockpanel.h"
#include "dockwidget.h"
#include "dockfloatingcontainer.h"
#include "dockoverlay.h"
#include "dockfocuscontroller.h"
#include "dockmanager.h"
#include "dockautohidecontainer.h"

#include <QWindowStateChangeEvent>

QX_DOCK_BEGIN_NAMESPACE

class DockWindowPrivate
{
public:
    QX_DECLARE_PUBLIC(DockWindow)
public:
    DockWindowPrivate();

    void init();
public:
    QList<DockContainer *> m_containers;
    QList<DockWidget *> m_dockWidgets;
    QList<QPointer<DockFloatingContainer>> m_floatingContainers;
    QVector<DockFloatingContainer *> m_uninitializedFloatingWidgets;
    DockWidget *m_centralWidget = nullptr;
    DockOverlay *m_containerOverlay;
    DockOverlay *m_panelOverlay;
    DockFocusController *m_focusController = nullptr;
    bool m_isLeavingMinimized = false;
    Qt::ToolButtonStyle m_toolBarStyleDocked = Qt::ToolButtonIconOnly;
    Qt::ToolButtonStyle m_toolBarStyleFloating = Qt::ToolButtonTextUnderIcon;
    QSize m_toolBarIconSizeDocked = QSize(16, 16);
    QSize m_toolBarIconSizeFloating = QSize(24, 24);
    DockWidget::DockWidgetFeatures m_lockedDockWidgetFeatures = DockWidget::DefaultDockWidgetFeatures;
};

DockWindowPrivate::DockWindowPrivate()
{
}

void DockWindowPrivate::init()
{
    Q_Q(DockWindow);

    m_containerOverlay = new DockOverlay(q, DockOverlay::PanelOverlayMode);
    m_panelOverlay = new DockOverlay(q, DockOverlay::ContainerOverlayMode);

    if (DockManager::testConfigFlag(DockManager::FocusHighlighting)) {
        m_focusController = new DockFocusController(q);
    }
}

DockWindow::DockWindow(QWidget *parent)
    : DockContainer{this, parent}
{
    QX_INIT_PRIVATE(DockWindow);
    createRootSplitter();
    createSideTabBarWidgets();
    Q_D(DockWindow);
    d->init();
    registerDockContainer(this);
}

DockWindow::~DockWindow()
{
    Q_D(DockWindow);
    std::vector<QPointer<DockPanel>> panels;
    for (int i = 0; i < dockPanelCount(); ++i) {
        panels.push_back(dockPanel(i));
    }
    for (auto p : panels) {
        if (!p || p->dockWindow() != this) {
            continue;
        }
        std::vector<QPointer<QWidget>> deleteWidgets;
        for (auto w : p->dockWidgets()) {
            deleteWidgets.push_back(w);
        }
        for (auto w : deleteWidgets) {
            delete w;
        }
    }

    auto floatingContainers = d->m_floatingContainers;
    for (auto c : floatingContainers) {
        c->deleteContent();
        delete c;
    }

    for (auto p : panels) {
        delete p;
    }

    QX_FINI_PRIVATE();
}

/**
 * Adds dockwidget into the given area.
 * If DockPanel is not null, then the area parameter indicates the area
 * into the DockPanel. If DockPanel is null, the Dockwidget will
 * be dropped into the container. If you would like to add a dock widget
 * tabified, then you need to add it to an existing dock panel object
 * into the CenterDockWidgetArea. The following code shows this:
 * \code
 * DockWindow->addDockWidget(Qx::CenterDockWidgetArea, newDockWidget,
 * 	   ExisitingPanel);
 * \endcode
 * \return Returns the dock panel that contains the new DockWidget
 */
DockPanel *DockWindow::addDockWidget(Qx::DockWidgetArea area, DockWidget *w, DockPanel *p, int index)
{
    Q_D(DockWindow);
    d->m_dockWidgets.append(w);
    DockContainer *container = p ? p->dockContainer() : this;
    if (container == nullptr) {
        container = this;
    }
    DockPanel *panel = container->addDockWidget(area, w, p, index);
    if (panel) {
        Q_EMIT dockWidgetAdded(w);
    }
    return panel;
}

/**
 * This function will add the given DockWidget to the given dock area as a new tab.
 * If no DockPanel exists for the given area identifier, a new DockPanel is created.
 */
DockPanel *DockWindow::addDockWidgetTab(Qx::DockWidgetArea area, DockWidget *w)
{
    DockPanel *panel = lastAddedDockPanel(area);
    if (panel) {
        return addDockWidget(Qx::CenterDockWidgetArea, w, panel);
    } else {
        return addDockWidget(area, w, nullptr);
    }
}

/**
 * This function will add the given DockWidget to the given DockPanel as a new tab.
 * If index is out of range, the tab is simply appended. Otherwise it is
 * inserted at the specified position.
 */
DockPanel *DockWindow::addDockWidgetTab(DockWidget *w, DockPanel *p, int index)
{
    return addDockWidget(Qx::CenterDockWidgetArea, w, p, index);
}

void DockWindow::removeDockWidget(DockWidget *w)
{
    Q_D(DockWindow);
    Q_EMIT dockWidgetAboutToBeRemoved(w);
    d->m_dockWidgets.removeAll(w);
    DockContainer::removeDockWidget(w);
    w->setDockWindow(nullptr);
    Q_EMIT dockWidgetRemoved(w);
}

DockAutoHideContainer *DockWindow::addAutoHideDockWidget(Qx::DockSideBarArea area, DockWidget *w)
{
    return addAutoHideDockWidgetToContainer(area, w, this);
}

DockAutoHideContainer *DockWindow::addAutoHideDockWidgetToContainer(Qx::DockSideBarArea area, DockWidget *w,
                                                                    DockContainer *container)
{
    Q_D(DockWindow);
    d->m_dockWidgets.append(w);
    auto c = container->createAndSetupAutoHideContainer(area, w);
    c->collapseView(true);

    Q_EMIT dockWidgetAdded(w);
    return c;
}

DockFloatingContainer *DockWindow::addDockWidgetFloating(DockWidget *w)
{
    Q_D(DockWindow);
    d->m_dockWidgets.append(w);
    DockPanel *oldPanel = w->dockPanel();
    if (oldPanel) {
        oldPanel->removeDockWidget(w);
    }

    w->setDockWindow(this);
    DockFloatingContainer *floatingWidget = new DockFloatingContainer(w);
    floatingWidget->resize(w->size());
    if (isVisible()) {
        floatingWidget->show();
    } else {
        d->m_uninitializedFloatingWidgets.append(floatingWidget);
    }
    Q_EMIT dockWidgetAdded(w);
    return floatingWidget;
}

const QList<DockContainer *> DockWindow::dockContainers() const
{
    Q_D(const DockWindow);
    return d->m_containers;
}

DockWidget *DockWindow::centralWidget() const
{
    Q_D(const DockWindow);
    return d->m_centralWidget;
}

DockPanel *DockWindow::setCentralWidget(DockWidget *widget)
{
    Q_D(DockWindow);
    if (!widget) {
        d->m_centralWidget = nullptr;
        return nullptr;
    }

    // Setting a new central widget is now allowed if there is already a central
    // widget or if there are already other dock widgets
    if (d->m_centralWidget) {
        qWarning("Setting a central widget not possible because there is already a central widget.");
        return nullptr;
    }

    // Setting a central widget is now allowed if there are already other
    // dock widgets.
    if (!d->m_dockWidgets.isEmpty()) {
        qWarning("Setting a central widget not possible - the central widget need to be the first "
                 "dock widget that is added to the dock window.");
        return nullptr;
    }

    widget->setFeature(DockWidget::DockWidgetClosable, false);
    widget->setFeature(DockWidget::DockWidgetMovable, false);
    widget->setFeature(DockWidget::DockWidgetFloatable, false);
    widget->setFeature(DockWidget::DockWidgetPinnable, false);
    d->m_centralWidget = widget;
    DockPanel *centralPanel = addDockWidget(Qx::CenterDockWidgetArea, widget);
    centralPanel->setDockAreaFlag(DockPanel::HideSingleWidgetTitleBar, true);
    return centralPanel;
}

bool DockWindow::isLeavingMinimizedState() const
{
    Q_D(const DockWindow);
    return d->m_isLeavingMinimized;
}

Qt::ToolButtonStyle DockWindow::dockWidgetToolBarStyle(DockWidget::State state) const
{
    Q_D(const DockWindow);
    if (DockWidget::StateFloating == state) {
        return d->m_toolBarStyleFloating;
    } else {
        return d->m_toolBarStyleDocked;
    }
}

void DockWindow::setDockWidgetToolBarStyle(Qt::ToolButtonStyle style, DockWidget::State state)
{
    Q_D(DockWindow);
    if (DockWidget::StateFloating == state) {
        d->m_toolBarStyleFloating = style;
    } else {
        d->m_toolBarStyleDocked = style;
    }
}

QSize DockWindow::dockWidgetToolBarIconSize(DockWidget::State state) const
{
    Q_D(const DockWindow);
    if (DockWidget::StateFloating == state) {
        return d->m_toolBarIconSizeFloating;
    } else {
        return d->m_toolBarIconSizeDocked;
    }
}

void DockWindow::setDockWidgetToolBarIconSize(const QSize &iconSize, DockWidget::State state)
{
    Q_D(DockWindow);
    if (DockWidget::StateFloating == state) {
        d->m_toolBarIconSizeFloating = iconSize;
    } else {
        d->m_toolBarIconSizeDocked = iconSize;
    }
}

DockWidget::DockWidgetFeatures DockWindow::globallyLockedDockWidgetFeatures() const
{
    Q_D(const DockWindow);
    return d->m_lockedDockWidgetFeatures;
}

void DockWindow::lockDockWidgetFeaturesGlobally(DockWidget::DockWidgetFeatures features)
{
    Q_D(DockWindow);
    // Limit the features to DockWidget::GloballyLockableFeatures
    features &= DockWidget::GloballyLockableFeatures;
    if (d->m_lockedDockWidgetFeatures == features) {
        return;
    }

    d->m_lockedDockWidgetFeatures = features;
    // Call the notifyFeaturesChanged() function for all dock widgets to update
    // the state of the close and detach buttons
    for (auto w : d->m_dockWidgets) {
        w->notifyFeaturesChanged();
    }
}

void DockWindow::setDockWidgetFocused(DockWidget *w)
{
    Q_D(DockWindow);
    if (d->m_focusController) {
        d->m_focusController->setDockWidgetFocused(w);
    }
}

void DockWindow::endLeavingMinimizedState()
{
    Q_D(DockWindow);
    d->m_isLeavingMinimized = false;
    this->activateWindow();
}

void DockWindow::registerDockContainer(DockContainer *container)
{
    Q_D(DockWindow);
    d->m_containers.append(container);
}

void DockWindow::removeDockContainer(DockContainer *container)
{
    Q_D(DockWindow);
    if (this != container) {
        d->m_containers.removeAll(container);
    }
}

void DockWindow::registerFloatingWidget(DockFloatingContainer *floatingWidget)
{
    Q_D(DockWindow);
    d->m_floatingContainers.append(floatingWidget);
    Q_EMIT floatingWidgetCreated(floatingWidget);
}

void DockWindow::removeFloatingWidget(DockFloatingContainer *floatingWidget)
{
    Q_D(DockWindow);
    d->m_floatingContainers.removeAll(floatingWidget);
}

DockOverlay *DockWindow::containerOverlay() const
{
    Q_D(const DockWindow);
    return d->m_containerOverlay;
}

DockOverlay *DockWindow::panelOverlay() const
{
    Q_D(const DockWindow);
    return d->m_panelOverlay;
}

void DockWindow::notifyDockAreaRelocation(QWidget *droppedWidget)
{
    Q_D(DockWindow);
    if (d->m_focusController) {
        d->m_focusController->notifyDockAreaRelocation(droppedWidget);
    }
}

void DockWindow::notifyFloatingWidgetDrop(DockFloatingContainer *floatingWidget)
{
    Q_D(DockWindow);
    if (d->m_focusController) {
        d->m_focusController->notifyFloatingWidgetDrop(floatingWidget);
    }
}

DockFocusController *DockWindow::dockFocusController() const
{
    Q_D(const DockWindow);
    return d->m_focusController;
}

bool DockWindow::eventFilter(QObject *obj, QEvent *e)
{
    Q_D(DockWindow);
    if (e->type() == QEvent::WindowStateChange) {
        QWindowStateChangeEvent *ev = static_cast<QWindowStateChangeEvent *>(e);
        if (ev->oldState().testFlag(Qt::WindowMinimized)) {
            d->m_isLeavingMinimized = true;
            QMetaObject::invokeMethod(this, "endLeavingMinimizedState", Qt::QueuedConnection);
        }
    }
    return Super::eventFilter(obj, e);
}

QX_DOCK_END_NAMESPACE
