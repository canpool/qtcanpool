/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "dockpanel_p.h"

QX_DOCK_BEGIN_NAMESPACE

/*
 * Update the auto hide button checked state based on if it's contained in an auto hide container or not
 */
void DockPanel::updateAutoHideButtonCheckState()
{
    auto autoHideButton = titleBarButton(Qx::TitleBarButtonAutoHide);
    autoHideButton->blockSignals(true);
    autoHideButton->setChecked(isAutoHide());
    autoHideButton->blockSignals(false);
}

void DockPanel::updateTitleBarButtonsToolTips()
{
    internal::setToolTip(titleBarButton(Qx::TitleBarButtonClose),
                         titleBar()->titleBarButtonToolTip(Qx::TitleBarButtonClose));
    internal::setToolTip(titleBarButton(Qx::TitleBarButtonAutoHide),
                         titleBar()->titleBarButtonToolTip(Qx::TitleBarButtonAutoHide));
}

/**
 * Call this function, if you already know, that the dock does not
 * contain any visible content (any open dock widgets).
 */
void DockPanel::hideAreaWithNoVisibleContent()
{
    this->toggleView(false);

    // Hide empty parent splitters
    auto splitter = parentSplitter();
    internal::hideEmptyParentSplitters(splitter);

    // Hide empty floating widget
    DockContainer *container = this->dockContainer();
    if (!container->isFloating() && !DockManager::testConfigFlag(DockManager::HideSingleCentralWidgetTitleBar)) {
        return;
    }

    updateTitleBarVisibility();
    auto topLevelWidget = container->topLevelDockWidget();
    auto floatingWidget = container->floatingWidget();
    if (topLevelWidget) {
        if (floatingWidget) {
            floatingWidget->updateWindowTitle();
        }
        DockWidget::emitTopLevelEventForWidget(topLevelWidget, true);
    } else if (container->openedDockPanels().isEmpty() && floatingWidget) {
        floatingWidget->hide();
    }
    if (isAutoHide()) {
        autoHideContainer()->hide();
    }
}

/**
 * Updates the dock panel layout and components visibility
 */
void DockPanel::updateTitleBarVisibility()
{
    Q_D(DockPanel);
    DockContainer *container = dockContainer();
    if (!container) {
        return;
    }
    if (!d->m_titleBar) {
        return;
    }
    bool isAutoHide = this->isAutoHide();
    if (!DockManager::testConfigFlag(DockManager::AlwaysShowTabs)) {
        bool hidden = container->hasTopLevelDockWidget() && container->isFloating();
        hidden |= (d->m_flags.testFlag(HideSingleWidgetTitleBar) && openDockWidgetsCount() == 1);
        hidden &= !isAutoHide;   // Titlebar must always be visible when auto hidden so it can be dragged
        d->m_titleBar->setVisible(!hidden);
    }
    if (isAutoHideFeatureEnabled()) {
        d->m_titleBar->showAutoHideControls(isAutoHide);
        updateTitleBarButtonVisibility(container->topLevelDockPanel() == this);
    }
}

/**
 * This is the internal private function for setting the current widget.
 * This function is called by the public setCurrentDockWidget() function
 * and by the dock window when restoring the state
 */
void DockPanel::internalSetCurrentDockWidget(DockWidget *w)
{
    int index = indexOf(w);
    if (index < 0) {
        return;
    }
    setCurrentIndex(index);
    // Set current index can show the widget without changing the close state, added to keep the close state consistent
    w->setClosedState(false);
}

/*
 * Update the title bar button visibility based on if it's top level or not
 */
void DockPanel::updateTitleBarButtonVisibility(bool isTopLevel)
{
    Q_D(DockPanel);
    d->updateTitleBarButtonVisibility(isTopLevel);
}

/**
 * Marks tabs menu to update
 */
void DockPanel::markTitleBarMenuOutdated()
{
    Q_D(DockPanel);
    if (d->m_titleBar) {
        d->m_titleBar->markTabsMenuOutdated();
    }
}

QX_DOCK_END_NAMESPACE
