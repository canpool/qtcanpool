/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 * **/
#include "dockwidget_p.h"
#include "dockwindow.h"
#include "dockpanel.h"
#include "docktab.h"
#include "dockmanager.h"
#include "dockutils.h"
#include "dockfloatingcontainer.h"
#include "docksidetab.h"
#include "dockautohidecontainer.h"

#include <QPointer>
#include <QAction>

QX_DOCK_BEGIN_NAMESPACE

/**
 * Returns the side tab widget for this dock, if this dock widget is in
 * a auto hide container. If it is not in a auto hide container, then this
 * function returns a nullptr,
 */
DockSideTab *DockWidget::sideTab() const
{
    Q_D(const DockWidget);
    return d->m_sideTab;
}

/**
 * Assign a side tab widget if this dock widget is an auto hide container
 */
void DockWidget::setSideTab(DockSideTab *sideTab)
{
    Q_D(DockWidget);
    d->m_sideTab = sideTab;
}

bool DockWidget::isAutoHide() const
{
    Q_D(const DockWidget);
    return !d->m_sideTab.isNull();
}

/**
 * Returns the auto hide dock container of this dock widget
 * or 0 if there is none
 */
DockAutoHideContainer *DockWidget::autoHideContainer() const
{
    Q_D(const DockWidget);

    if (!d->m_panel) {
        return nullptr;
    }

    return d->m_panel->autoHideContainer();
}

/**
 * Returns the auto hide side bar area or Qx::DockSideBarNone if, this is not
 * an autohide dock widget
 */
Qx::DockSideBarArea DockWidget::autoHideArea() const
{
    return isAutoHide() ? autoHideContainer()->sideBarArea() : Qx::DockSideBarNone;
}

/**
 * Only used when the feature flag DeleteContentOnClose is set.
 * Using the flag and setting a widget factory allows to free the resources
 * of the widget of your application while retaining the position the next
 * time you want to show your widget, unlike the flag DockWidgetDeleteOnClose
 * which deletes the dock widget itself. Since we keep the dock widget, all
 * regular features of QxDock should work as normal, including saving and
 * restoring the state of the docking system and using perspectives.
 */
void DockWidget::setWidgetFactory(DockWidget::FactoryFunc createWidget, DockWidget::WidgetInsertMode insertMode)
{
    Q_D(DockWidget);
    if (d->m_factory) {
        delete d->m_factory;
    }

    d->m_factory = new DockWidgetPrivate::WidgetFactory{createWidget, insertMode};
}

#ifndef QT_NO_TOOLTIP
void DockWidget::setTabToolTip(const QString &text)
{
    Q_D(DockWidget);
    if (d->m_tab) {
        d->m_tab->setToolTip(text);
    }
    if (d->m_toggleViewAction) {
        d->m_toggleViewAction->setToolTip(text);
    }
    if (d->m_panel) {
        d->m_panel->markTitleBarMenuOutdated();   // update tabs menu
    }
}
#endif

/**
 * This property controls whether the dock widget is open or closed.
 * The toogleViewAction triggers this slot
 */
void DockWidget::toggleView(bool open)
{
    Q_D(DockWidget);
    QAction *a = qobject_cast<QAction *>(sender());
    if (a == d->m_toggleViewAction && !d->m_toggleViewAction->isCheckable()) {
        open = true;
    }
    // If the dock widget state is different, then we really need to toggle
    // the state. If we are in the right state, then we simply make this
    // dock widget the current dock widget
    auto container = this->autoHideContainer();
    if (d->m_closed != !open) {
        toggleViewInternal(open);
    } else if (open && d->m_panel && !container) {
        raise();
    }

    if (open && container) {
        container->collapseView(false);
    }
}

/**
 * This function will delete the dock widget and its content from the
 * docking system
 */
void DockWidget::deleteDockWidget()
{
    Q_D(DockWidget);
    if (d->m_window) {
        d->m_window->removeDockWidget(this);
    }
    deleteLater();
    d->m_closed = true;
}

/**
 * Closes the dock widget.
 * The function forces closing of the dock widget even for CustomCloseHandling.
 */
void DockWidget::closeDockWidget()
{
    closeDockWidgetInternal(true);
}

/**
 * Request closing of the dock widget.
 * For DockWidget with default close handling, the function does the same
 * like closeDockWidget() but if the flas CustomCloseHandling is set,
 * the function only emits the closeRequested() signal.
 */
void DockWidget::requestCloseDockWidget()
{
    if (features().testFlag(DockWidget::DockWidgetDeleteOnClose) ||
        features().testFlag(DockWidget::CustomCloseHandling)) {
        closeDockWidgetInternal(false);
    } else {
        toggleView(false);
    }
}

/**
 * Sets the dock widget into auto hide mode if this feature is enabled
 * via DockManager::setAutoHideFlags(DockManager::AutoHideFeatureEnabled)
 */
void DockWidget::setAutoHide(bool enable, Qx::DockSideBarArea area, int tabIndex)
{
    if (!DockManager::testAutoHideConfigFlag(DockManager::AutoHideFeatureEnabled)) {
        return;
    }

    // Do nothing if nothing changes
    if (enable == isAutoHide() && area == autoHideArea()) {
        return;
    }

    auto panel = dockPanel();

    if (!enable) {
        panel->setAutoHide(false);
    } else if (isAutoHide()) {
        // DockSideBarNone means "no particular side was asked for". For a
        // widget that is already pinned the current side is the answer, and
        // it has to be substituted here rather than passed on: a container
        // only ever builds a side bar for the four sides, so None would come
        // back as a null side bar to move the widget to.
        autoHideContainer()->moveToNewSideBarArea(area == Qx::DockSideBarNone ? autoHideArea() : area);
    } else {
        auto location = (Qx::DockSideBarNone == area) ? panel->calculateSideBarArea() : area;
        dockContainer()->createAndSetupAutoHideContainer(location, this, tabIndex);
    }
}

/**
 * Switches the dock widget to auto hide mode or vice versa depending on its
 * current state.
 */
void DockWidget::toggleAutoHide(Qx::DockSideBarArea area)
{
    if (!DockManager::testAutoHideConfigFlag(DockManager::AutoHideFeatureEnabled)) {
        return;
    }

    setAutoHide(!isAutoHide(), area);
}

/**
 * Makes this dock widget the current tab in its dock panel.
 * The function only has an effect, if the dock widget is open. A call
 * to this function will not toggle the view, so if it is closed,
 * nothing will happen
 */
void DockWidget::setAsCurrentTab()
{
    Q_D(DockWidget);
    if (d->m_panel && !isClosed()) {
        d->m_panel->setCurrentDockWidget(this);
    }
}

/**
 * Brings the dock widget to the front
 * This means:
 * 	- If the dock widget is tabbed with other dock widgets but its tab is not current, it's made current.
 * 	- If the dock widget is floating, QWindow::raise() is called.
 * 	This only applies if the dock widget is already open. If closed, does nothing.
 */
void DockWidget::raise()
{
    if (isClosed()) {
        return;
    }

    setAsCurrentTab();
    if (isInFloatingContainer()) {
        auto floatingWindow = window();
        floatingWindow->raise();
        floatingWindow->activateWindow();
    }
}

/**
 * Shows the widget in full-screen mode.
 * Normally this function only affects windows. To make the interface
 * compatible to QDockWidget, this function also maximizes a floating
 * dock widget.
 *  * \note Full-screen mode works fine under Windows, but has certain
 * problems (doe not work) under X (Linux). These problems are due to
 * limitations of the ICCCM protocol that specifies the communication
 * between X11 clients and the window manager. ICCCM simply does not
 * understand the concept of non-decorated full-screen windows.
 */
void DockWidget::showFullScreen()
{
    if (isFloating()) {
        dockContainer()->floatingWidget()->showFullScreen();
    } else {
        Super::showFullScreen();
    }
}

/**
 * This function complements showFullScreen() to restore the widget
 * after it has been in full screen mode.
 */
void DockWidget::showNormal()
{
    if (isFloating()) {
        dockContainer()->floatingWidget()->showNormal();
    } else {
        Super::showNormal();
    }
}

/**
 * This function will make a docked widget floating
 */
void DockWidget::setFloating()
{
    Q_D(DockWidget);
    if (isClosed()) {
        return;
    }

    if (this->isAutoHide()) {
        dockPanel()->setFloating();
    } else {
        d->m_tab->detachDockWidget();
    }
}

/**
 * Assigns the dock window that manages this dock widget
 */
void DockWidget::setDockWindow(DockWindow *window)
{
    Q_D(DockWidget);
    d->m_window = window;
    if (!window) {
        return;
    }

    if (d->m_toolBarStyleSource == DockWidget::ToolBarStyleFromDockWindow) {
        d->setToolBarStyleFromDockWindow();
    }
}

/**
 * If this dock widget is inserted into a dock panel, the dock panel will
 * be registered on this widget via this function. If a dock widget is
 * removed from a dock panel, this function will be called with nullptr
 * value.
 */
void DockWidget::setDockPanel(DockPanel *panel)
{
    Q_D(DockWidget);
    d->m_panel = panel;
    d->m_toggleViewAction->setChecked(panel != nullptr && !this->isClosed());
    setParent(panel);
}

void DockWidget::setClosedState(bool closed)
{
    Q_D(DockWidget);
    d->m_closed = closed;
}

/**
 * Call this function to emit a topLevelChanged() signal and to update
 * the dock panel tool bar visibility
 */
void DockWidget::emitTopLevelEventForWidget(DockWidget *topLevelDockWidget, bool floating)
{
    if (topLevelDockWidget) {
        topLevelDockWidget->dockPanel()->updateTitleBarVisibility();
        topLevelDockWidget->emitTopLevelChanged(floating);
    }
}

/**
 * Use this function to emit a top level changed event.
 * Do never use emit topLevelChanged(). Always use this function because
 * it only emits a signal if the floating state has really changed
 */
void DockWidget::emitTopLevelChanged(bool floating)
{
    Q_D(DockWidget);
    if (floating != d->m_isFloatingTopLevel) {
        d->m_isFloatingTopLevel = floating;
        Q_EMIT topLevelChanged(floating);
    }
}

/**
 * Internal toggle view function that does not check if the widget
 * already is in the given state
 */
void DockWidget::toggleViewInternal(bool open)
{
    Q_D(DockWidget);
    DockContainer *container = dockContainer();
    DockWidget *topLevelDockWidgetBefore = container ? container->topLevelDockWidget() : nullptr;

    d->m_closed = !open;
    if (open) {
        d->showDockWidget();
    } else {
        d->hideDockWidget();
    }
    d->m_toggleViewAction->blockSignals(true);
    d->m_toggleViewAction->setChecked(open);
    d->m_toggleViewAction->blockSignals(false);
    if (d->m_panel) {
        d->m_panel->toggleDockWidgetView(this, open);
        // The auto-hide side tab used to be toggled from here as well. It is
        // not needed, and was disabled because showing it from this call could
        // bring the tab up while the window itself was still hidden (during
        // state restore): the side tab belongs to the side bar, and the side
        // bar is what shows and hides it - DockSideBar::insertTab() shows the
        // bar, removeTab() hides it once the last tab is gone. Toggling the
        // widget only decides whether the panel is open.
    }

    if (open && topLevelDockWidgetBefore) {
        DockWidget::emitTopLevelEventForWidget(topLevelDockWidgetBefore, false);
    }

    // Here we need to call the dockContainer() function again, because if
    // this dock widget was unassigned before the call to showDockWidget() then
    // it has a dock container now
    container = dockContainer();
    DockWidget *topLevelDockWidgetAfter = container ? container->topLevelDockWidget() : nullptr;
    DockWidget::emitTopLevelEventForWidget(topLevelDockWidgetAfter, true);
    DockFloatingContainer *floatingContainer = container ? container->floatingWidget() : nullptr;
    if (floatingContainer) {
        floatingContainer->updateWindowTitle();
    }

    if (!open) {
        Q_EMIT closed();
    }
    Q_EMIT viewToggled(open);
}

/**
 * Internal close dock widget implementation.
 * The function returns true if the dock widget has been closed or hidden
 */
bool DockWidget::closeDockWidgetInternal(bool forceClose)
{
    Q_D(DockWidget);

    if (!forceClose) {
        Q_EMIT closeRequested();
    }
    if (!forceClose && features().testFlag(DockWidget::CustomCloseHandling)) {
        return false;
    }
    if (features().testFlag(DockWidget::DockWidgetDeleteOnClose)) {
        // If the dock widget is floating, then we check if we also need to
        // delete the floating widget
        if (isFloating()) {
            DockFloatingContainer *floatingWidget = internal::findParent<DockFloatingContainer *>(this);
            if (floatingWidget->dockWidgets().count() == 1) {
                floatingWidget->deleteLater();
            } else {
                floatingWidget->hide();
            }
        }

        if (d->m_panel && d->m_panel->isAutoHide()) {
            d->m_panel->autoHideContainer()->cleanupAndDelete();
        }
        deleteDockWidget();
        Q_EMIT closed();
    } else {
        toggleView(false);
    }
    return true;
}

/**
 * This is a helper function for the dock manager to flag this widget
 * as unassigned.
 * When calling the restore function, it may happen, that the saved state
 * contains less dock widgets then currently available. All widgets whose
 * data is not contained in the saved state, are flagged as unassigned
 * after the restore process. If the user shows an unassigned dock widget,
 * a floating widget will be created to take up the dock widget.
 */
void DockWidget::flagAsUnassigned()
{
    Q_D(DockWidget);
    d->m_closed = true;
    setParent(d->m_window);
    setVisible(false);
    setDockPanel(nullptr);
    tab()->setParent(this);
}

QX_DOCK_END_NAMESPACE
