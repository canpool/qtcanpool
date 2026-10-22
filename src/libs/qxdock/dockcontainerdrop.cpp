/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "dockcontainer_p.h"
#include "dockwindow.h"
#include "docksplitter.h"
#include "dockpanel.h"
#include "dockutils.h"
#include "dockmanager.h"
#include "dockfloatingcontainer.h"
#include "dockoverlay.h"
#include "dockautohidecontainer.h"
#include "docksidebar.h"
#include "docksidetab.h"

#include <QGridLayout>
#include <QTimer>
#include <QApplication>
#include <QPointer>
#include <QTextStream>

QX_DOCK_BEGIN_NAMESPACE

void DockContainerPrivate::dropIntoContainer(DockFloatingContainer *floatingWidget, Qx::DockWidgetArea area)
{
    auto param = internal::dockAreaInsertParameters(area);
    DockContainer *floatingDockContainer = floatingWidget->dockContainer();
    auto newPanels = floatingDockContainer->findChildren<DockPanel *>(QString(), Qt::FindChildrenRecursively);
    auto splitter = m_rootSplitter;

    if (m_panels.count() <= 1) {
        splitter->setOrientation(param.orientation());
    } else if (splitter->orientation() != param.orientation()) {
        auto newSplitter = this->newSplitter(param.orientation());
        QLayoutItem *li = m_layout->replaceWidget(splitter, newSplitter);
        newSplitter->addWidget(splitter);
        updateSplitterHandles(newSplitter);
        splitter = newSplitter;
        delete li;
    }

    // Now we can insert the floating widget content into this container
    auto floatingSplitter = floatingDockContainer->rootSplitter();
    if (floatingSplitter->count() == 1) {
        insertWidgetIntoSplitter(splitter, floatingSplitter->widget(0), param.append());
        updateSplitterHandles(splitter);
    } else if (floatingSplitter->orientation() == param.orientation()) {
        int insertIndex = param.append() ? splitter->count() : 0;
        while (floatingSplitter->count()) {
            splitter->insertWidget(insertIndex++, floatingSplitter->widget(0));
            updateSplitterHandles(splitter);
        }
    } else {
        insertWidgetIntoSplitter(splitter, floatingSplitter, param.append());
    }

    m_rootSplitter = splitter;
    addDockPanelsToList(newPanels);

    // If we dropped the floating widget into the main dock container that does
    // not contain any dock widgets, then splitter is invisible and we need to
    // show it to display the docked widgets
    if (!splitter->isVisible()) {
        splitter->show();
    }
}

void DockContainerPrivate::dropIntoCenterOfPanel(DockFloatingContainer *floatingWidget, DockPanel *targetPanel,
                                                 int tabIndex)
{
    DockContainer *floatingContainer = floatingWidget->dockContainer();
    auto newDockWidgets = floatingContainer->dockWidgets();
    auto topLevelPanel = floatingContainer->topLevelDockPanel();
    int newCurrentIndex = -1;
    tabIndex = qMax(0, tabIndex);

    // If the floating widget contains only one single dock are, then the
    // current dock widget of the dock area will also be the future current
    // dock widget in the drop area.
    if (topLevelPanel) {
        newCurrentIndex = topLevelPanel->currentIndex();
    }

    for (int i = 0; i < newDockWidgets.count(); ++i) {
        DockWidget *dockWidget = newDockWidgets[i];
        targetPanel->insertDockWidget(tabIndex + i, dockWidget, false);
        // If the floating widget contains multiple visible dock areas, then we
        // simply pick the first visible open dock widget and make it
        // the current one.
        if (newCurrentIndex < 0 && !dockWidget->isClosed()) {
            newCurrentIndex = i;
        }
    }
    targetPanel->setCurrentIndex(newCurrentIndex + tabIndex);
    targetPanel->updateTitleBarVisibility();
    return;
}

void DockContainerPrivate::dropIntoPanel(DockFloatingContainer *floatingWidget, DockPanel *targetPanel,
                                         Qx::DockWidgetArea area, int tabIndex)
{
    // Dropping into center means all dock widgets in the dropped floating
    // widget will become tabs of the drop area
    if (Qx::CenterDockWidgetArea == area) {
        dropIntoCenterOfPanel(floatingWidget, targetPanel, tabIndex);
        return;
    }

    DockContainer *floatingContainer = floatingWidget->dockContainer();
    auto param = internal::dockAreaInsertParameters(area);
    auto newDockpanels = floatingContainer->findChildren<DockPanel *>(QString(), Qt::FindChildrenRecursively);
    auto targetSplitter = targetPanel->parentSplitter();
    int areaIndex = targetSplitter->indexOf(targetPanel);
    auto floatingSplitter = floatingContainer->rootSplitter();
    if (targetSplitter->orientation() == param.orientation()) {
        auto sizes = targetSplitter->sizes();
        int targetAreaSize = (param.orientation() == Qt::Horizontal) ? targetPanel->width() : targetPanel->height();
        bool adjustSplitterSizes = true;
        if ((floatingSplitter->orientation() != param.orientation()) && floatingSplitter->count() > 1) {
            targetSplitter->insertWidget(areaIndex + param.insertOffset(), floatingSplitter);
            updateSplitterHandles(targetSplitter);
        } else {
            adjustSplitterSizes = (floatingSplitter->count() == 1);
            int insertIndex = areaIndex + param.insertOffset();
            while (floatingSplitter->count()) {
                targetSplitter->insertWidget(insertIndex++, floatingSplitter->widget(0));
                updateSplitterHandles(targetSplitter);
            }
        }

        if (adjustSplitterSizes) {
            int size = (targetAreaSize - targetSplitter->handleWidth()) / 2;
            sizes[areaIndex] = size;
            sizes.insert(areaIndex, size);
            targetSplitter->setSizes(sizes);
        }
    } else {
        QSplitter *newSplitter = this->newSplitter(param.orientation());
        int targetAreaSize = (param.orientation() == Qt::Horizontal) ? targetPanel->width() : targetPanel->height();
        bool adjustSplitterSizes = true;
        if ((floatingSplitter->orientation() != param.orientation()) && floatingSplitter->count() > 1) {
            newSplitter->addWidget(floatingSplitter);
            updateSplitterHandles(newSplitter);
        } else {
            adjustSplitterSizes = (floatingSplitter->count() == 1);
            while (floatingSplitter->count()) {
                newSplitter->addWidget(floatingSplitter->widget(0));
                updateSplitterHandles(newSplitter);
            }
        }

        // Save the sizes before insertion and restore it later to prevent
        // shrinking of existing area
        auto sizes = targetSplitter->sizes();
        insertWidgetIntoSplitter(newSplitter, targetPanel, !param.append());
        updateSplitterHandles(newSplitter);
        if (adjustSplitterSizes) {
            int size = targetAreaSize / 2;
            newSplitter->setSizes({size, size});
        }
        targetSplitter->insertWidget(areaIndex, newSplitter);
        targetSplitter->setSizes(sizes);
        updateSplitterHandles(targetSplitter);
    }

    addDockPanelsToList(newDockpanels);
}

void DockContainerPrivate::moveToContainer(QWidget *widget, Qx::DockWidgetArea area)
{
    Q_Q(DockContainer);
    DockWidget *droppedDockWidget = qobject_cast<DockWidget *>(widget);
    DockPanel *droppedPanel = qobject_cast<DockPanel *>(widget);
    DockPanel *newPanel;

    if (droppedDockWidget) {
        newPanel = new DockPanel(m_window, q);
        DockPanel *oldPanel = droppedDockWidget->dockPanel();
        if (oldPanel) {
            oldPanel->removeDockWidget(droppedDockWidget);
        }
        newPanel->addDockWidget(droppedDockWidget);
    } else {
        // We check, if we insert the dropped widget into the same place that
        // it already has and do nothing, if it is the same place. It would
        // also work without this check, but it looks nicer with the check
        // because there will be no layout updates
        auto splitter = droppedPanel->parentSplitter();
        auto insertParam = internal::dockAreaInsertParameters(area);
        if (splitter == m_rootSplitter && insertParam.orientation() == splitter->orientation()) {
            if (insertParam.append() && splitter->lastWidget() == droppedPanel) {
                return;
            } else if (!insertParam.append() && splitter->firstWidget() == droppedPanel) {
                return;
            }
        }
        droppedPanel->dockContainer()->removeDockPanel(droppedPanel);
        newPanel = droppedPanel;
    }

    addDockPanel(newPanel, area);
    m_lastAddedPanelCache[areaIdToIndex(area)] = newPanel;
}

void DockContainerPrivate::moveIntoCenterOfSection(QWidget *widget, DockPanel *targetPanel, int tabIndex)
{
    auto droppedDockWidget = qobject_cast<DockWidget *>(widget);
    auto droppedPanel = qobject_cast<DockPanel *>(widget);

    tabIndex = qMax(0, tabIndex);
    if (droppedDockWidget) {
        DockPanel *oldPanel = droppedDockWidget->dockPanel();
        if (oldPanel == targetPanel) {
            return;
        }

        if (oldPanel) {
            oldPanel->removeDockWidget(droppedDockWidget);
        }
        targetPanel->insertDockWidget(tabIndex, droppedDockWidget, true);
    } else {
        QList<DockWidget *> newDockWidgets = droppedPanel->dockWidgets();
        int newCurrentIndex = droppedPanel->currentIndex();
        for (int i = 0; i < newDockWidgets.count(); ++i) {
            DockWidget *w = newDockWidgets[i];
            targetPanel->insertDockWidget(tabIndex + i, w, false);
        }
        targetPanel->setCurrentIndex(tabIndex + newCurrentIndex);
        droppedPanel->dockContainer()->removeDockPanel(droppedPanel);
        droppedPanel->deleteLater();
    }

    targetPanel->updateTitleBarVisibility();
    return;
}

void DockContainerPrivate::moveToNewSection(QWidget *widget, DockPanel *targetPanel, Qx::DockWidgetArea area,
                                            int tabIndex)
{
    Q_Q(DockContainer);
    // Dropping into center means all dock widgets in the dropped floating
    // widget will become tabs of the drop area
    if (Qx::CenterDockWidgetArea == area) {
        moveIntoCenterOfSection(widget, targetPanel, tabIndex);
        return;
    }

    DockWidget *droppedDockWidget = qobject_cast<DockWidget *>(widget);
    DockPanel *droppedPanel = qobject_cast<DockPanel *>(widget);
    DockPanel *newPanel;
    if (droppedDockWidget) {
        newPanel = new DockPanel(m_window, q);
        DockPanel *oldPanel = droppedDockWidget->dockPanel();
        if (oldPanel) {
            oldPanel->removeDockWidget(droppedDockWidget);
        }
        newPanel->addDockWidget(droppedDockWidget);
    } else {
        droppedPanel->dockContainer()->removeDockPanel(droppedPanel);
        newPanel = droppedPanel;
    }

    auto insertParam = internal::dockAreaInsertParameters(area);
    auto targetSplitter = targetPanel->parentSplitter();
    int panelIndex = targetSplitter->indexOf(targetPanel);
    auto sizes = targetSplitter->sizes();
    if (targetSplitter->orientation() == insertParam.orientation()) {
        int targetPanelSize =
            (insertParam.orientation() == Qt::Horizontal) ? targetPanel->width() : targetPanel->height();
        targetSplitter->insertWidget(panelIndex + insertParam.insertOffset(), newPanel);
        updateSplitterHandles(targetSplitter);
        int size = (targetPanelSize - targetSplitter->handleWidth()) / 2;
        sizes[panelIndex] = size;
        sizes.insert(panelIndex, size);
    } else {
        int targetPanelSize =
            (insertParam.orientation() == Qt::Horizontal) ? targetPanel->width() : targetPanel->height();
        QSplitter *newSplitter = this->newSplitter(insertParam.orientation());
        newSplitter->addWidget(targetPanel);
        insertWidgetIntoSplitter(newSplitter, newPanel, insertParam.append());
        updateSplitterHandles(newSplitter);
        int size = targetPanelSize / 2;
        newSplitter->setSizes({size, size});
        targetSplitter->insertWidget(panelIndex, newSplitter);
        updateSplitterHandles(targetSplitter);
    }
    targetSplitter->setSizes(sizes);

    addDockPanelsToList({newPanel});
}

void DockContainerPrivate::dropIntoAutoHideSideBar(DockFloatingContainer *floatingWidget, Qx::DockWidgetArea area)
{
    Q_Q(DockContainer);
    auto sideBarArea = internal::toSideBarArea(area);
    auto newPanels = floatingWidget->findChildren<DockPanel *>(QString(), Qt::FindChildrenRecursively);
    int tabIndex = m_window->containerOverlay()->tabIndexUnderCursor();
    for (auto panel : newPanels) {
        auto widgets = panel->dockWidgets();
        for (auto w : widgets) {
            q->createAndSetupAutoHideContainer(sideBarArea, w, tabIndex++);
        }
    }
}

void DockContainerPrivate::moveToAutoHideSideBar(QWidget *widget, Qx::DockWidgetArea area, int tabIndex)
{
    Q_Q(DockContainer);
    DockWidget *droppedWidget = qobject_cast<DockWidget *>(widget);
    DockPanel *droppedPanel = qobject_cast<DockPanel *>(widget);
    auto sideBarArea = internal::toSideBarArea(area);

    if (droppedWidget) {
        if (q == droppedWidget->dockContainer()) {
            droppedWidget->setAutoHide(true, sideBarArea, tabIndex);
        } else {
            q->createAndSetupAutoHideContainer(sideBarArea, droppedWidget, tabIndex);
        }
    } else {
        if (q == droppedPanel->dockContainer()) {
            droppedPanel->setAutoHide(true, sideBarArea, tabIndex);
        } else {
            for (const auto dockWidget : droppedPanel->openedDockWidgets()) {
                if (!dockWidget->features().testFlag(DockWidget::DockWidgetPinnable)) {
                    continue;
                }
                q->createAndSetupAutoHideContainer(sideBarArea, dockWidget, tabIndex++);
            }
        }
    }
}

/**
 * This function forces the dock container widget to update handles of splitters
 * based on resize modes of dock widgets contained in the container.
 */
void DockContainer::updateSplitterHandles(QSplitter *splitter)
{
    Q_D(DockContainer);
    d->updateSplitterHandles(splitter);
}

/**
 * Drop floating widget into the container
 */
void DockContainer::dropFloatingWidget(DockFloatingContainer *floatingWidget, const QPoint &targetPos)
{
    Q_D(DockContainer);
    DockWidget *singleDroppedDockWidget = floatingWidget->topLevelDockWidget();
    DockWidget *singleDockWidget = topLevelDockWidget();
    auto dropArea = Qx::InvalidDockWidgetArea;
    auto containerDropArea = d->m_window->containerOverlay()->dropAreaUnderCursor();
    bool dropped = false;

    DockPanel *panel = dockPanelAt(targetPos);
    // mouse is over dock area
    if (panel) {
        auto dropOverlay = d->m_window->panelOverlay();
        dropOverlay->setAllowedAreas(panel->allowedAreas());
        dropArea = dropOverlay->showOverlay(panel);
        if (containerDropArea != Qx::InvalidDockWidgetArea && containerDropArea != dropArea) {
            dropArea = Qx::InvalidDockWidgetArea;
        }

        if (dropArea != Qx::InvalidDockWidgetArea) {
            int tabIndex = d->m_window->panelOverlay()->tabIndexUnderCursor();
            d->dropIntoPanel(floatingWidget, panel, dropArea, tabIndex);
            dropped = true;
        }
    }

    // mouse is over container or auto hide side bar
    if (Qx::InvalidDockWidgetArea == dropArea && Qx::InvalidDockWidgetArea != containerDropArea) {
        if (internal::isSideBarArea(containerDropArea)) {
            d->dropIntoAutoHideSideBar(floatingWidget, containerDropArea);
        } else {
            d->dropIntoContainer(floatingWidget, containerDropArea);
        }
        dropped = true;
    }
    // Remove the auto hide widgets from the floatingWidget and insert
    // them into this widget
    for (auto autoHideWidget : floatingWidget->dockContainer()->autoHideWidgets()) {
        auto sideBar = autoHideSideBar(autoHideWidget->sideBarArea());
        sideBar->addAutoHideWidget(autoHideWidget);
    }

    if (dropped) {
        floatingWidget->finishDropOperation();

        // If we dropped a floating widget with only one single dock widget, then we
        // drop a top level widget that changes from floating to docked now
        DockWidget::emitTopLevelEventForWidget(singleDroppedDockWidget, false);

        // If there was a top level widget before the drop, then it is not top
        // level widget anymore
        DockWidget::emitTopLevelEventForWidget(singleDockWidget, false);
    }

    window()->activateWindow();
    if (singleDroppedDockWidget) {
        d->m_window->notifyDockAreaRelocation(singleDroppedDockWidget);
    }
    d->m_window->notifyFloatingWidgetDrop(floatingWidget);
}

/**
 * Drop a dock panel or a dock widget given in widget parameter.
 * If the targetPanel is a nullptr, then the dropArea indicates
 * the drop area for the container. If the given targetPanel is not
 * a nullptr, then the dropArea indicates the drop area in the given
 * targetPanel
 */
void DockContainer::dropWidget(QWidget *widget, Qx::DockWidgetArea dropArea, DockPanel *targetPanel, int tabIndex)
{
    Q_D(DockContainer);
    DockWidget *singleDockWidget = topLevelDockWidget();
    if (targetPanel) {
        d->moveToNewSection(widget, targetPanel, dropArea, tabIndex);
    } else if (internal::isSideBarArea(dropArea)) {
        d->moveToAutoHideSideBar(widget, dropArea, tabIndex);
    } else {
        d->moveToContainer(widget, dropArea);
    }

    // If there was a top level widget before the drop, then it is not top
    // level widget anymore
    DockWidget::emitTopLevelEventForWidget(singleDockWidget, false);

    window()->activateWindow();
    d->m_window->notifyDockAreaRelocation(widget);
}

void DockContainer::createSideTabBarWidgets()
{
    Q_D(DockContainer);
    if (!DockManager::testAutoHideConfigFlag(DockManager::AutoHideFeatureEnabled)) {
        return;
    }

    {
        auto area = Qx::DockSideBarLeft;
        d->m_sideTabBarWidgets[area] = new DockSideBar(this, area);
        d->m_layout->addWidget(d->m_sideTabBarWidgets[area], 1, 0);
    }

    {
        auto area = Qx::DockSideBarRight;
        d->m_sideTabBarWidgets[area] = new DockSideBar(this, area);
        d->m_layout->addWidget(d->m_sideTabBarWidgets[area], 1, 2);
    }

    {
        auto area = Qx::DockSideBarBottom;
        d->m_sideTabBarWidgets[area] = new DockSideBar(this, area);
        d->m_layout->addWidget(d->m_sideTabBarWidgets[area], 2, 1);
    }

    {
        auto area = Qx::DockSideBarTop;
        d->m_sideTabBarWidgets[area] = new DockSideBar(this, area);
        d->m_layout->addWidget(d->m_sideTabBarWidgets[area], 0, 1);
    }
}

/**
 * Creates and initializes a dockwidget auto hide container into the given area.
 * Initializing inserts the tabs into the side tab widget and hides it
 * Returns nullptr if you try and insert into an area where the configuration is not enabled
 */
DockAutoHideContainer *DockContainer::createAndSetupAutoHideContainer(Qx::DockSideBarArea area, DockWidget *w,
                                                                      int tabIndex)
{
    Q_D(DockContainer);
    if (!DockManager::testAutoHideConfigFlag(DockManager::AutoHideFeatureEnabled)) {
        Q_ASSERT_X(false, "DockContainer::createAndSetupAutoHideContainer", "Requested area does not exist in config");
        return nullptr;
    }
    if (d->m_window != w->dockWindow()) {
        w->setDockWindow(d->m_window);   // Auto hide Dock Container needs a valid dock window
    }

    return autoHideSideBar(area)->insertDockWidget(tabIndex, w);
}

void DockContainer::registerAutoHideWidget(DockAutoHideContainer *autoHideWidget)
{
    Q_D(DockContainer);
    d->m_autoHideWidgets.append(autoHideWidget);
    Q_EMIT autoHideWidgetCreated(autoHideWidget);
}

void DockContainer::removeAutoHideWidget(DockAutoHideContainer *autoHideWidget)
{
    Q_D(DockContainer);
    d->m_autoHideWidgets.removeAll(autoHideWidget);
}

void DockContainer::handleAutoHideWidgetEvent(QEvent *e, QWidget *w)
{
    Q_D(DockContainer);
    if (!DockManager::testAutoHideConfigFlag(DockManager::AutoHideShowOnMouseOver)) {
        return;
    }

    if (dockWindow()->isRestoringState()) {
        return;
    }

    auto autoHideTab = qobject_cast<DockSideTab *>(w);
    if (autoHideTab) {
        switch (e->type()) {
        case QEvent::Enter:
            if (!autoHideTab->dockWidget()->isVisible()) {
                d->m_delayedAutoHideTab = autoHideTab;
                d->m_delayedAutoHideTimer.start();
            } else {
                d->m_delayedAutoHideTimer.stop();
            }
            break;

        case QEvent::MouseButtonPress:
            d->m_delayedAutoHideTimer.stop();
            break;

        case QEvent::Leave:
            if (autoHideTab->dockWidget()->isVisible()) {
                d->m_delayedAutoHideTab = autoHideTab;
                d->m_delayedAutoHideTimer.start();
            } else {
                d->m_delayedAutoHideTimer.stop();
            }
            break;

        default:
            break;
        }
        return;
    }

    auto autoHideContainer = qobject_cast<DockAutoHideContainer *>(w);
    if (autoHideContainer) {
        switch (e->type()) {
        case QEvent::Enter:
        case QEvent::Hide:
            d->m_delayedAutoHideTimer.stop();
            break;

        case QEvent::Leave:
            if (autoHideContainer->isVisible()) {
                d->m_delayedAutoHideTab = autoHideContainer->autoHideTab();
                d->m_delayedAutoHideTimer.start();
            }
            break;

        default:
            break;
        }
        return;
    }
}

QX_DOCK_END_NAMESPACE
