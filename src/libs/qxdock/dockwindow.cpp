/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "dockwindow.h"
#include "dockwindow_p.h"
#include "dockcontainer.h"
#include "dockpanel.h"
#include "dockwidget.h"
#include "dockfloatingcontainer.h"
#include "dockoverlay.h"
#include "dockfocuscontroller.h"
#include "dockmanager.h"
#include "dockautohidecontainer.h"
#include "docksplitter.h"

#include <QSettings>
#include <QPointer>
#include <QAction>

QX_DOCK_BEGIN_NAMESPACE

DockWindow::DockWindow(QWidget *parent)
    : DockContainer{this, parent}
{
    QX_INIT_PRIVATE(DockWindow);
    createRootSplitter();
    createSideTabBarWidgets();
    Q_D(DockWindow);
    d->init();
    registerDockContainer(this);

    // Accessibility (B2): the window is the dock landmark, so it is what registers
    // the factory that gives the panel, tab, tab bar and side bar their roles.
    installDockAccessibility();

    window()->installEventFilter(this);

    // A dock widget can arrive as a native drag (see DockDrag) as well as by
    // mouse; the window is what receives it, so it has to be a drop target.
    setAcceptDrops(true);

#if QX_DOCK_X11
    connect(qApp, &QApplication::focusWindowChanged, [](QWindow *focusWindow) {
        // bring modal dialogs to foreground to ensure that they are in front of any
        // floating dock widget
        if (focusWindow && focusWindow->isModal()) {
            focusWindow->raise();
        }
    });
#endif
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
 * into the DockPanel. If DockPanel is null, the DockWidget will
 * be dropped into the container. If you would like to add a dock widget
 * tabified, then you need to add it to an existing dock panel object
 * into the CenterDockWidgetArea. The following code shows this:
 * \code
 * DockWindow->addDockWidget(Qx::CenterDockWidgetArea, newDockWidget,
 * 	   exisitingPanel);
 * \endcode
 * \param area The dock area the dock widget is added to
 * \param w The dock widget to add
 * \param p The dock panel to add the dock widget into, or null to use the
 *          container itself
 * \param index Tab index, used only when DockPanel is not null and
 *              area is CenterDockWidgetArea
 * \return Returns the dock panel that contains the new DockWidget
 */
DockPanel *DockWindow::addDockWidget(Qx::DockWidgetArea area, DockWidget *w, DockPanel *p, int index)
{
    Q_D(DockWindow);
    d->mapDockWidget(w);
    DockContainer *container = p ? p->dockContainer() : this;
    if (container == nullptr) {
        container = this;
        p = nullptr;   // panel is not contained by any container, can not be used again
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

/**
 * Adds dockwidget into the given container.
 * This allows you to place the dock widget into a container, even if that
 * container does not yet contain a DockWidget.
 * \return Returns the dock panel that contains the new DockWidget
 */
DockPanel *DockWindow::addDockWidgetToContainer(Qx::DockWidgetArea area, DockWidget *w, DockContainer *container)
{
    Q_D(DockWindow);
    d->mapDockWidget(w);
    auto panel = container->addDockWidget(area, w);
    Q_EMIT dockWidgetAdded(w);
    return panel;
}

void DockWindow::removeDockWidget(DockWidget *w)
{
    Q_D(DockWindow);
    Q_EMIT dockWidgetAboutToBeRemoved(w);
    d->m_dockWidgetsMap.remove(w->objectName());
    DockContainer::removeDockWidget(w);
    w->setDockWindow(nullptr);
    Q_EMIT dockWidgetRemoved(w);
}

/**
 * Searches for a registered dock widget with the given objectName
 * \return Return the found dock widget or nullptr if a dock widget with the
 * given name is not registered
 */
DockWidget *DockWindow::findDockWidget(const QString &objectName) const
{
    Q_D(const DockWindow);
    return d->m_dockWidgetsMap.value(objectName, nullptr);
}

/**
 * Adds an Auto-Hide widget to the dock window container pinned to
 * the given side bar area.
 * \return Returns the DockAutoHideContainer that contains the new DockWidget
 */
DockAutoHideContainer *DockWindow::addAutoHideDockWidget(Qx::DockSideBarArea area, DockWidget *w)
{
    return addAutoHideDockWidgetToContainer(area, w, this);
}

/**
 * Adds an Auto-Hide widget to the given container pinned to
 * the given side bar area in this container.
 * \return Returns the DockAutoHideContainer that contains the new DockWidget
 */
DockAutoHideContainer *DockWindow::addAutoHideDockWidgetToContainer(Qx::DockSideBarArea area, DockWidget *w,
                                                                    DockContainer *container)
{
    Q_D(DockWindow);
    if (area == Qx::DockSideBarNone) {
        return nullptr;
    }
    d->mapDockWidget(w);
    auto c = container->createAndSetupAutoHideContainer(area, w);
    c->collapseView(true);

    Q_EMIT dockWidgetAdded(w);
    return c;
}

/**
 * Adds the given DockWidget floating and returns the created
 * DockFloatingContainer instance.
 */
DockFloatingContainer *DockWindow::addDockWidgetFloating(DockWidget *w)
{
    Q_D(DockWindow);
    d->mapDockWidget(w);
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

/**
 * Returns the list of all active and visible dock containers
 * Dock containers are the main dock manager and all floating widgets
 */
const QList<DockContainer *> DockWindow::dockContainers() const
{
    Q_D(const DockWindow);
    return d->m_containers;
}

/**
 * This function returns a readable reference to the internal dock
 * widgets map so that it is possible to iterate over all dock widgets
 */
QMap<QString, DockWidget *> DockWindow::dockWidgetsMap() const
{
    Q_D(const DockWindow);
    return d->m_dockWidgetsMap;
}

DockWidget *DockWindow::centralWidget() const
{
    Q_D(const DockWindow);
    return d->m_centralWidget;
}

/**
 * Adds dockwidget widget into the central area and marks it as central widget.
 *
 * If central widget is set, it will be the only dock widget
 * that will resize with the dock container. A central widget if not
 * movable, floatable or closable and the titlebar of the central
 * dock panel is not visible.
 *
 * If the given widget could be set as central widget, the function returns
 * the created dock panel. If the widget could not be set, because there
 * is already a central widget, this function returns a nullptr.
 *
 * To clear the central widget, pass a nullptr to the function.
 *
 * \note Setting a central widget is only possible if no other dock widgets
 * have been registered before. That means, this function should be the
 * first function that you call before you add other dock widgets.
 * \retval != 0 The dock panel that contains the central widget
 * \retval nullptr Indicates that the given widget can not be set as central
 *         widget because there is already a central widget.
 */
DockPanel *DockWindow::setCentralWidget(DockWidget *widget)
{
    Q_D(DockWindow);
    if (!widget) {
        d->m_centralWidget = nullptr;
        return nullptr;
    }

    // Setting a new central widget is not allowed if there is already a central
    // widget or if there are already other dock widgets
    if (d->m_centralWidget) {
        qWarning("Setting a central widget not possible because there is already a central widget.");
        return nullptr;
    }

    // Setting a central widget is not allowed if there are already other dock widgets.
    if (!d->m_dockWidgetsMap.isEmpty()) {
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

/**
 * This function returns true, if the DockWindow window is restoring from
 * minimized state.
 * The DockWindow is in this state starting from the QWindowStateChangeEvent
 * that signals the state change from minimized to normal until
 * endLeavingMinimizedState() function is called.
 */
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

/**
 * This function sets the tool button style for the given dock widget state.
 * It is possible to switch the tool button style depending on the state.
 * If a dock widget is floating, then here are more space and it is
 * possible to select a style that requires more space like
 * Qt::ToolButtonTextUnderIcon. For the docked state Qt::ToolButtonIconOnly
 * might be better.
 */
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

/**
 * This function sets the tool button icon size for the given state.
 * If a dock widget is floating, there is more space and increasing the
 * icon size is possible. For docked widgets, small icon sizes, eg. 16 x 16
 * might be better.
 */
void DockWindow::setDockWidgetToolBarIconSize(const QSize &iconSize, DockWidget::State state)
{
    Q_D(DockWindow);
    if (DockWidget::StateFloating == state) {
        d->m_toolBarIconSizeFloating = iconSize;
    } else {
        d->m_toolBarIconSizeDocked = iconSize;
    }
}

/**
 * Returns all dock widget features that are globally locked by the dock window.
 * Globally locked features are removed from the features of all dock widgets.
 */
DockWidget::DockWidgetFeatures DockWindow::globallyLockedDockWidgetFeatures() const
{
    Q_D(const DockWindow);
    return d->m_lockedDockWidgetFeatures;
}

/**
 * Globally Lock features of all dock widgets to "freeze" the current
 * workspace layout.
 * For example, it is now possible to lock the workspace to avoid
 * accidentally dragging a docked view. Locking wasn't possible before.
 * So, users had to manually dock it back to the desired place after
 * each accidental undock.
 * You can use a combination of the following feature flags:
 * - DockWidget::DockWidgetClosable
 * - DockWidget::DockWidgetMovable
 * - DockWidget::DockWidgetFloatable
 * - DockWidget::DockWidgetPinable
 *
 * To clear the locked features, you can use DockWidget::NoDockWidgetFeatures
 * The following code shows how to lock and unlock dock widget features globally.
 *
 * \code
 * DockWindow->lockDockWidgetFeaturesGlobally();
 * DockWindow->lockDockWidgetFeaturesGlobally(DockWidget::NoDockWidgetFeatures);
 * \endcode
 */
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
    for (auto w : d->m_dockWidgetsMap) {
        w->notifyFeaturesChanged();
    }
}

/**
 * Returns the sizes of the splitter that contains the dock panel.
 *
 * If there is no splitter that contains the panel, an empty list will be
 * returned.
 */
QList<int> DockWindow::splitterSizes(DockPanel *panel) const
{
    if (panel) {
        auto splitter = panel->parentSplitter();
        if (splitter) {
            return splitter->sizes();
        }
    }
    return QList<int>();
}

/**
 * Update the sizes of a splitter
 * Programmatically updates the sizes of a given splitter by calling
 * QSplitter::setSizes(). The splitter will be the splitter that
 * contains the supplied dock panel widget. If there is not splitter
 * that contains the dock panel, or the sizes supplied does not match
 * the number of children of the splitter, this method will have no
 * effect.
 */
void DockWindow::setSplitterSizes(DockPanel *panel, const QList<int> &sizes)
{
    if (!panel) {
        return;
    }

    auto splitter = panel->parentSplitter();
    if (splitter && splitter->count() == sizes.count()) {
        splitter->setSizes(sizes);
    }
}

/**
 * Saves the current perspective to the internal list of perspectives.
 * A perspective is the current state of the dock window assigned
 * with a certain name. This makes it possible for the user,
 * to switch between different perspectives quickly.
 * If a perspective with the given name already exists, then
 * it will be overwritten with the new state.
 */
void DockWindow::addPerspective(const QString &uniquePrespectiveName)
{
    Q_D(DockWindow);
    d->m_perspectives.insert(uniquePrespectiveName, saveState());
    Q_EMIT perspectiveListChanged();
}

void DockWindow::removePerspective(const QString &name)
{
    removePerspectives({name});
}

void DockWindow::removePerspectives(const QStringList &names)
{
    Q_D(DockWindow);
    int count = 0;
    for (const auto &name : names) {
        count += d->m_perspectives.remove(name);
    }

    if (count) {
        Q_EMIT perspectivesRemoved();
        Q_EMIT perspectiveListChanged();
    }
}

QStringList DockWindow::perspectiveNames() const
{
    Q_D(const DockWindow);
    return d->m_perspectives.keys();
}

void DockWindow::savePerspectives(QSettings &settings) const
{
    settings.beginWriteArray("Perspectives", d_ptr->m_perspectives.size());
    int i = 0;
    for (auto it = d_ptr->m_perspectives.constBegin(); it != d_ptr->m_perspectives.constEnd(); ++it) {
        settings.setArrayIndex(i);
        settings.setValue("Name", it.key());
        settings.setValue("State", it.value());
        ++i;
    }
    settings.endArray();
}

void DockWindow::loadPerspectives(QSettings &settings)
{
    Q_D(DockWindow);
    d->m_perspectives.clear();
    int size = settings.beginReadArray("Perspectives");
    if (!size) {
        settings.endArray();
        return;
    }

    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        QString name = settings.value("Name").toString();
        QByteArray data = settings.value("State").toByteArray();
        if (name.isEmpty() || data.isEmpty()) {
            continue;
        }

        d->m_perspectives.insert(name, data);
    }

    settings.endArray();
    Q_EMIT perspectiveListChanged();
    Q_EMIT perspectiveListLoaded();
}

/**
 * Request a focus change to the given dock widget.
 * This function only has an effect, if the flag DockManager::FocusHighlighting
 * is enabled
 */
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

    // A container that is a child of this window is destroyed by ~QWidget(),
    // which runs after ~DockWindow() deleted the private data. Dropping it from
    // a list that is going away anyway is a no-op, not a reason to crash.
    if (d && this != container) {
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

QX_DOCK_END_NAMESPACE
