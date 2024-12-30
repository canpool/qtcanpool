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
#include "dockutils.h"
#include "dockstatereader.h"
#include "docksplitter.h"

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
#include "linux/dockfloatingtitlebar.h"
#endif

#include <QWindowStateChangeEvent>
#include <QXmlStreamWriter>
#include <QFile>
#include <QSettings>
#include <QTextStream>
#include <QPointer>
#include <QDebug>
#include <QApplication>
#include <QAction>

QX_DOCK_BEGIN_NAMESPACE

enum StateFileVersion {
    InitialVersion = 0,         //!< InitialVersion
    Version1 = 1,               //!< Version1
    CurrentVersion = Version1   //!< CurrentVersion
};

class DockWindowPrivate
{
public:
    QX_DECLARE_PUBLIC(DockWindow)
public:
    DockWindowPrivate();

    void init();
    void loadStylesheet();

    void mapDockWidget(DockWidget *w);

    bool checkFormat(const QByteArray &state, int version);
    bool restoreStateFromXml(const QByteArray &state, int version, bool testing = internal::Restore);
    bool restoreContainer(int index, DockStateReader &stream, bool testing);
    bool restoreState(const QByteArray &state, int version);

    void restoreDockWidgetsOpenState();
    void restoreDockAreasIndices();
    void emitTopLevelEvents();
    void hideFloatingWidgets();
    void markDockWidgetsDirty();
public:
    QList<DockContainer *> m_containers;
    QMap<QString, DockWidget *> m_dockWidgetsMap;
    QList<QPointer<DockFloatingContainer>> m_floatingContainers;
    QList<QPointer<DockFloatingContainer>> m_hiddenFloatingContainers;
    QVector<DockFloatingContainer *> m_uninitializedFloatingWidgets;
    QMap<QString, QByteArray> m_perspectives;
    DockWidget *m_centralWidget = nullptr;
    DockOverlay *m_containerOverlay;
    DockOverlay *m_panelOverlay;
    DockFocusController *m_focusController = nullptr;
    bool m_isLeavingMinimized = false;
    Qt::ToolButtonStyle m_toolBarStyleDocked = Qt::ToolButtonIconOnly;
    Qt::ToolButtonStyle m_toolBarStyleFloating = Qt::ToolButtonTextUnderIcon;
    QSize m_toolBarIconSizeDocked = QSize(16, 16);
    QSize m_toolBarIconSizeFloating = QSize(24, 24);
    DockWidget::DockWidgetFeatures m_lockedDockWidgetFeatures;
    bool m_restoringState = false;
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

#ifndef QX_DOCK_DEBUG
    loadStylesheet();
#endif
}

void DockWindowPrivate::loadStylesheet()
{
    Q_Q(DockWindow);
    QString result;
    QString fileName = ":/qxdock/res/stylesheets/";
    fileName += DockManager::testConfigFlag(DockManager::FocusHighlighting) ? "focus_highlighting" : "default";
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    fileName += "_linux";
#endif
    fileName += ".css";
    QFile styleSheetFile(fileName);
    styleSheetFile.open(QIODevice::ReadOnly);
    QTextStream styleSheetStream(&styleSheetFile);
    result = styleSheetStream.readAll();
    styleSheetFile.close();
    q->setStyleSheet(result);
}

void DockWindowPrivate::mapDockWidget(DockWidget *w)
{
    const QString &objname = w->objectName();
    DockWidget *oldWidget = m_dockWidgetsMap.value(objname, nullptr);
    if (oldWidget) {
        qWarning() << QString("Object name (%1) of DockWidget already exists.").arg(objname);
    }
    m_dockWidgetsMap.insert(objname, w);
}

bool DockWindowPrivate::checkFormat(const QByteArray &state, int version)
{
    return restoreStateFromXml(state, version, internal::RestoreTesting);
}

bool DockWindowPrivate::restoreStateFromXml(const QByteArray &state, int version, bool testing)
{
    Q_UNUSED(version);
    Q_Q(DockWindow);

    if (state.isEmpty()) {
        return false;
    }
    DockStateReader s(state);
    s.readNextStartElement();
    if (s.name() != QLatin1String("QxDock")) {
        return false;
    }
    bool ok;
    int v = s.attributes().value("Version").toInt(&ok);
    if (!ok || v > CurrentVersion) {
        return false;
    }
    s.setFileVersion(v);

    // Older files do not support UserVersion but we still want to load them so
    // we first test if the attribute exists
    if (!s.attributes().value("UserVersion").isEmpty()) {
        v = s.attributes().value("UserVersion").toInt(&ok);
        if (!ok || v != version) {
            return false;
        }
    }

    bool result = true;

    if (m_centralWidget) {
        const auto centralWidgetAttribute = s.attributes().value("CentralWidget");
        // If we have a central widget but a state without central widget, then
        // something is wrong.
        if (centralWidgetAttribute.isEmpty()) {
            qWarning() << "Dock window has central widget but saved state does not have central widget.";
            return false;
        }

        // If the object name of the central widget does not match the name of the
        // saved central widget, the something is wrong
        if (m_centralWidget->objectName() != centralWidgetAttribute.toString()) {
            qWarning() << "Object name of central widget does not match name of central widget in saved state.";
            return false;
        }
    }

    int dockContainerCount = 0;
    while (s.readNextStartElement()) {
        if (s.name() == QLatin1String("Container")) {
            result = restoreContainer(dockContainerCount, s, testing);
            if (!result) {
                break;
            }
            dockContainerCount++;
        }
    }

    if (!testing) {
        // Delete remaining empty floating widgets
        int floatingWidgetIndex = dockContainerCount - 1;
        for (int i = floatingWidgetIndex; i < m_floatingContainers.count(); ++i) {
            DockFloatingContainer *floatingWidget = m_floatingContainers[i];
            if (!floatingWidget)
                continue;
            q->removeDockContainer(floatingWidget->dockContainer());
            floatingWidget->deleteLater();
        }
    }

    return result;
}

bool DockWindowPrivate::restoreContainer(int index, DockStateReader &stream, bool testing)
{
    Q_Q(DockWindow);
    if (testing) {
        index = 0;
    }

    bool result = false;
    if (index >= m_containers.count()) {
        DockFloatingContainer *floatingWidget = new DockFloatingContainer(q);
        result = floatingWidget->restoreState(stream, testing);
    } else {
        auto container = m_containers[index];
        if (container->isFloating()) {
            result = container->floatingWidget()->restoreState(stream, testing);
        } else {
            result = container->restoreState(stream, testing);
        }
    }

    return result;
}

bool DockWindowPrivate::restoreState(const QByteArray &state, int version)
{
    QByteArray s = state.startsWith("<?xml") ? state : qUncompress(state);
    if (!checkFormat(s, version)) {
        return false;
    }

    // Hide updates of floating widgets from use
    hideFloatingWidgets();
    markDockWidgetsDirty();

    if (!restoreStateFromXml(s, version)) {
        return false;
    }

    restoreDockWidgetsOpenState();
    restoreDockAreasIndices();
    emitTopLevelEvents();

    return true;
}

void DockWindowPrivate::restoreDockWidgetsOpenState()
{
    // All dock widgets, that have not been processed in the restore state
    // function are invisible to the user now and have no assigned dock area
    // They do not belong to any dock container, until the user toggles the
    // toggle view action the next time
    for (auto dockWidget : m_dockWidgetsMap) {
        if (dockWidget->property(internal::DirtyProperty).toBool()) {
            // If the DockWidget is an auto hide widget that is not assigned yet,
            // then we need to delete the auto hide container now
            if (dockWidget->isAutoHide()) {
                dockWidget->autoHideContainer()->cleanupAndDelete();
            }
            dockWidget->flagAsUnassigned();
            Q_EMIT dockWidget->viewToggled(false);
        } else {
            dockWidget->toggleViewInternal(!dockWidget->property(internal::ClosedProperty).toBool());
        }
    }
}

void DockWindowPrivate::restoreDockAreasIndices()
{
    Q_Q(DockWindow);
    // Now all dock panels are properly restored and we setup the index of
    // The dock panels because the previous toggleView() action has changed
    // the dock panel index
    for (auto dockContainer : m_containers) {
        for (int i = 0; i < dockContainer->dockPanelCount(); ++i) {
            DockPanel *panel = dockContainer->dockPanel(i);
            QString dockWidgetName = panel->property("currentDockWidget").toString();
            DockWidget *dockWidget = nullptr;
            if (!dockWidgetName.isEmpty()) {
                dockWidget = q->findDockWidget(dockWidgetName);
            }

            if (!dockWidget || dockWidget->isClosed()) {
                int index = panel->indexOfFirstOpenDockWidget();
                if (index < 0) {
                    continue;
                }
                panel->setCurrentIndex(index);
            } else {
                panel->internalSetCurrentDockWidget(dockWidget);
            }
        }
    }
}

void DockWindowPrivate::emitTopLevelEvents()
{
    // Finally we need to send the topLevelChanged() signals for all dock
    // widgets if top level changed
    for (auto dockContainer : m_containers) {
        DockWidget *topLevelWidget = dockContainer->topLevelDockWidget();
        if (topLevelWidget) {
            topLevelWidget->emitTopLevelChanged(true);
        } else {
            for (int i = 0; i < dockContainer->dockPanelCount(); ++i) {
                auto panel = dockContainer->dockPanel(i);
                for (auto dockWidget : panel->dockWidgets()) {
                    dockWidget->emitTopLevelChanged(false);
                }
            }
        }
    }
}

void DockWindowPrivate::hideFloatingWidgets()
{
    // Hide updates of floating widgets from user
    for (auto floatingWidget : m_floatingContainers) {
        if (floatingWidget) {
            floatingWidget->hide();
        }
    }
}

void DockWindowPrivate::markDockWidgetsDirty()
{
    for (auto dockWidget : m_dockWidgetsMap) {
        dockWidget->setProperty(internal::DirtyProperty, true);
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

    window()->installEventFilter(this);

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
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
        p = nullptr; // panel is not contained by any container, can not be used again
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
 * accidentally dragging a docked view. Locking wasn’t possible before.
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
 * Saves the current state of the dockwindow and all its dock widgets
 * into the returned QByteArray.
 * The XmlMode enables / disables the auto formatting for the XmlStreamWriter.
 * If auto formatting is enabled, the output is intended and line wrapped.
 * The XmlMode XmlAutoFormattingDisabled is better if you would like to have
 * a more compact XML output - i.e. for storage in ini files.
 * The version number is stored as part of the data.
 * To restore the saved state, pass the return value and version number
 * to restoreState().
 * \see restoreState()
 */
QByteArray DockWindow::saveState(int version) const
{
    Q_D(const DockWindow);
    QByteArray xmldata;
    QXmlStreamWriter s(&xmldata);
    auto configFlags = DockManager::configFlags();
    s.setAutoFormatting(configFlags.testFlag(DockManager::XmlAutoFormattingEnabled));

    s.writeStartDocument();
    s.writeStartElement("QxDock");

    s.writeAttribute("Version", QString::number(CurrentVersion));
    s.writeAttribute("UserVersion", QString::number(version));
    s.writeAttribute("Containers", QString::number(d->m_containers.count()));
    if (d->m_centralWidget) {
        s.writeAttribute("CentralWidget", d->m_centralWidget->objectName());
    }
    for (auto container : d->m_containers) {
        container->saveState(s);
    }

    s.writeEndElement();
    s.writeEndDocument();

    return configFlags.testFlag(DockManager::XmlCompressionEnabled) ? qCompress(xmldata, 9) : xmldata;
}

/**
 * Restores the state of this dockwindow dockwidgets.
 * The version number is compared with that stored in state. If they do
 * not match, the dockmanager's state is left unchanged, and this function
 * returns false; otherwise, the state is restored, and this function
 * returns true.
 * \see saveState()
 */
bool DockWindow::restoreState(const QByteArray &state, int version)
{
    Q_D(DockWindow);
    // Prevent multiple calls as long as state is not restore. This may
    // happen, if QApplication::processEvents() is called somewhere
    if (d->m_restoringState) {
        return false;
    }

    // We hide the complete dock window here. Restoring the state means
    // that DockWidgets are removed from the DockArea internal stack layout
    // which in turn means, that each time a widget is removed the stack
    // will show and raise the next available widget which in turn
    // triggers show events for the dock widgets. To avoid this we hide the
    // dock window. Because there will be no processing of application
    // events until this function is finished, the user will not see this
    // hiding
    bool isHidden = this->isHidden();
    if (!isHidden) {
        hide();
    }
    d->m_restoringState = true;
    Q_EMIT restoringState();
    bool result = d->restoreState(state, version);
    d->m_restoringState = false;
    if (!isHidden) {
        show();
    }
    Q_EMIT stateRestored();
    return result;
}

/**
 * This function returns true between the restoringState() and
 * stateRestored() signals.
 */
bool DockWindow::isRestoringState() const
{
    Q_D(const DockWindow);
    return d->m_restoringState;
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

void DockWindow::openPerspective(const QString &perspectiveName)
{
    Q_D(DockWindow);
    const auto iterator = d->m_perspectives.find(perspectiveName);
    if (d->m_perspectives.end() == iterator) {
        return;
    }

    Q_EMIT openingPerspective(perspectiveName);
    restoreState(iterator.value());
    Q_EMIT perspectiveOpened(perspectiveName);
}

/**
 * hide DockWindow and all floating widgets. Calling regular QWidget::hide()
 * hides the DockWindow but not the floating widgets;
 */
void DockWindow::hideWindowAndFloatingWidgets()
{
    Q_D(DockWindow);
    hide();

    d->m_hiddenFloatingContainers.clear();
    // Hide updates of floating widgets from user
    for (auto floatingWidget : d->m_floatingContainers) {
        if (floatingWidget->isVisible()) {
            QList<DockWidget *> visibleWidgets;
            for (auto dockWidget : floatingWidget->dockWidgets()) {
                if (dockWidget->toggleViewAction()->isChecked())
                    visibleWidgets.push_back(dockWidget);
            }

            // save as floating widget to be shown when DockWindow will be shown back
            d->m_hiddenFloatingContainers.push_back(floatingWidget);
            floatingWidget->hide();

            // hiding floating widget automatically marked contained DockWidgets as hidden
            // but they must remain marked as visible as we want them to be restored visible
            // when DockWindow will be shown back
            for (auto dockWidget : visibleWidgets) {
                dockWidget->toggleViewAction()->setChecked(true);
            }
        }
    }
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

void DockWindow::restoreHiddenFloatingWidgets()
{
    Q_D(DockWindow);
    if (d->m_hiddenFloatingContainers.isEmpty()) {
        return;
    }

    // Restore floating widgets that were hidden upon hideWindowAndFloatingWidgets
    for (auto floatingWidget : d->m_hiddenFloatingContainers) {
        bool hasDockWidgetVisible = false;

        // Needed to prevent DockFloatingContainer being shown empty
        // Could make sense to move this to DockFloatingContainer::showEvent(QShowEvent *event)
        // if experiencing DockFloatingContainer being shown empty in other situations, but let's keep
        // it here for now to make sure changes to fix Issue #380 does not impact existing behaviours
        for (auto dockWidget : floatingWidget->dockWidgets()) {
            if (dockWidget->toggleViewAction()->isChecked()) {
                dockWidget->toggleView(true);
                hasDockWidgetVisible = true;
            }
        }

        if (hasDockWidgetVisible) {
            floatingWidget->show();
        }
    }

    d->m_hiddenFloatingContainers.clear();
}

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
bool DockWindow::eventFilter(QObject *obj, QEvent *e)
{
    Q_D(DockWindow);
    // Emulate Qt:Tool behaviour.
    // Required because on some WMs Tool windows can't be maximized.

    // Window always on top of the MainWindow.
    if (e->type() == QEvent::WindowActivate) {
        for (auto _window : d->m_floatingContainers) {
            if (!_window->isVisible() || window()->isMinimized()) {
                continue;
            }
            // setWindowFlags(Qt::WindowStaysOnTopHint) will hide the window and thus requires a show call.
            // This then leads to flickering and a nasty endless loop (also buggy behaviour on Ubuntu).
            // So we just do it ourself.
            if (QGuiApplication::platformName() == QLatin1String("xcb")) {
                internal::xcb_update_prop(true, _window->window()->winId(), "_NET_WM_STATE", "_NET_WM_STATE_ABOVE",
                                          "_NET_WM_STATE_STAYS_ON_TOP");
            } else {
                _window->setWindowFlag(Qt::WindowStaysOnTopHint, true);
            }
        }
    } else if (e->type() == QEvent::WindowDeactivate) {
        for (auto _window : d->m_floatingContainers) {
            if (!_window->isVisible() || window()->isMinimized()) {
                continue;
            }

            if (QGuiApplication::platformName() == QLatin1String("xcb")) {
                internal::xcb_update_prop(false, _window->window()->winId(), "_NET_WM_STATE", "_NET_WM_STATE_ABOVE",
                                          "_NET_WM_STATE_STAYS_ON_TOP");
            } else {
                _window->setWindowFlag(Qt::WindowStaysOnTopHint, false);
            }
            _window->raise();
        }
    }

    // Sync minimize with MainWindow
    if (e->type() == QEvent::WindowStateChange) {
        for (auto _window : d->m_floatingContainers) {
            if (!_window->isVisible()) {
                continue;
            }

            if (window()->isMinimized()) {
                _window->showMinimized();
            } else {
                _window->setWindowState(_window->windowState() & (~Qt::WindowMinimized));
            }
        }
        if (!window()->isMinimized()) {
            QApplication::setActiveWindow(window());
        }
    }
    return Super::eventFilter(obj, e);
}
#else
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
#endif

void DockWindow::showEvent(QShowEvent *e)
{
    Q_D(DockWindow);
    Super::showEvent(e);

    restoreHiddenFloatingWidgets();
    if (d->m_uninitializedFloatingWidgets.empty()) {
        return;
    }

    for (auto floatingWidget : d->m_uninitializedFloatingWidgets) {
        // Check, if someone closed a floating dock widget before the dock window is shown
        if (floatingWidget->dockContainer()->hasOpenDockPanels()) {
            floatingWidget->show();
        }
    }
    d->m_uninitializedFloatingWidgets.clear();
}

QX_DOCK_END_NAMESPACE
