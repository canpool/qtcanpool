/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "dockfocuscontroller.h"
#include "dockwindow.h"
#include "dockwidget.h"
#include "dockpanel.h"
#include "dockutils.h"
#include "docktab.h"
#include "dockfloatingcontainer.h"
#include "docktitlebar.h"
#include "dockmanager.h"

#include <QApplication>

QX_DOCK_BEGIN_NAMESPACE

static const char *const s_focusedDockWidgetProperty = "FocusedDockWidget";

static void updateDockWidgetFocusStyle(DockWidget *widget, bool focused)
{
    widget->setProperty("focused", focused);
    widget->tab()->setProperty("focused", focused);
    widget->tab()->updateStyle();
    internal::repolishStyle(widget);
}

static void updateDockPanelFocusStyle(DockPanel *panel, bool focused)
{
    panel->setProperty("focused", focused);
    internal::repolishStyle(panel);
    internal::repolishStyle(panel->titleBar());
}

class DockFocusControllerPrivate
{
public:
    QX_DECLARE_PUBLIC(DockFocusController)
public:
    DockFocusControllerPrivate();
    void updateDockWidgetFocus(DockWidget *dockWidget);
public:
    DockWindow *m_window;
    QPointer<DockWidget> m_focusedDockWidget = nullptr;
    QPointer<DockWidget> m_oldFocusedDockWidget = nullptr;
    QPointer<DockPanel> m_focusedPanel = nullptr;
    bool m_tabPressed = false;
    bool m_forceFocusChangedSignal = false;
};

DockFocusControllerPrivate::DockFocusControllerPrivate()
{
}

void DockFocusControllerPrivate::updateDockWidgetFocus(DockWidget *dockWidget)
{
    if (!dockWidget)
        return;
    if (!dockWidget->features().testFlag(DockWidget::DockWidgetFocusable)) {
        return;
    }
    Q_Q(DockFocusController);

    QWindow *window = nullptr;
    auto DockContainer = dockWidget->dockContainer();
    if (DockContainer) {
        window = DockContainer->window()->windowHandle();
    }

    if (window) {
        window->setProperty(s_focusedDockWidgetProperty, QVariant::fromValue(QPointer<DockWidget>(dockWidget)));
    }
    DockPanel *newFocusedPanel = nullptr;
    if (m_focusedDockWidget) {
        updateDockWidgetFocusStyle(m_focusedDockWidget, false);
    }

    DockWidget *old = m_focusedDockWidget;
    m_focusedDockWidget = dockWidget;
    updateDockWidgetFocusStyle(m_focusedDockWidget, true);
    newFocusedPanel = m_focusedDockWidget->dockPanel();
    if (newFocusedPanel && (m_focusedPanel != newFocusedPanel)) {
        if (m_focusedPanel) {
            QObject::disconnect(m_focusedPanel, SIGNAL(viewToggled(bool)), q, SLOT(onFocusedDockAreaViewToggled(bool)));
            updateDockPanelFocusStyle(m_focusedPanel, false);
        }

        m_focusedPanel = newFocusedPanel;
        updateDockPanelFocusStyle(m_focusedPanel, true);
        QObject::connect(m_focusedPanel, SIGNAL(viewToggled(bool)), q, SLOT(onFocusedDockAreaViewToggled(bool)));
    }

    DockFloatingContainer *newFloatingWidget = nullptr;
    DockContainer = m_focusedDockWidget->dockContainer();
    if (DockContainer) {
        newFloatingWidget = DockContainer->floatingWidget();
    }

    if (newFloatingWidget) {
        newFloatingWidget->setProperty(s_focusedDockWidgetProperty,
                                       QVariant::fromValue(QPointer<DockWidget>(dockWidget)));
    }

    if (old == dockWidget && !m_forceFocusChangedSignal) {
        return;
    }

    m_forceFocusChangedSignal = false;
    if (dockWidget->isVisible()) {
        Q_EMIT m_window->focusedDockWidgetChanged(old, dockWidget);
    } else {
        m_oldFocusedDockWidget = old;
        QObject::connect(dockWidget, SIGNAL(visibilityChanged(bool)), q, SLOT(onDockWidgetVisibilityChanged(bool)));
    }
}

DockFocusController::DockFocusController(DockWindow *window)
    : QObject(window)
{
    QX_INIT_PRIVATE(DockFocusController);
    Q_D(DockFocusController);
    d->m_window = window;

    connect(QApplication::instance(), SIGNAL(focusChanged(QWidget *, QWidget *)), this,
            SLOT(onApplicationFocusChanged(QWidget *, QWidget *)));
    connect(QApplication::instance(), SIGNAL(focusWindowChanged(QWindow *)), this,
            SLOT(onFocusWindowChanged(QWindow *)));
}

DockFocusController::~DockFocusController()
{
    QX_FINI_PRIVATE();
}

void DockFocusController::notifyDockAreaRelocation(QWidget *droppedWidget)
{
    Q_D(DockFocusController);

    if (d->m_window->isRestoringState()) {
        return;
    }

    DockWidget *dockWidget = qobject_cast<DockWidget *>(droppedWidget);
    if (!dockWidget) {
        DockPanel *panel = qobject_cast<DockPanel *>(droppedWidget);
        if (panel) {
            dockWidget = panel->currentDockWidget();
        }
    }

    if (!dockWidget) {
        return;
    }

    d->m_forceFocusChangedSignal = true;
    DockManager::setWidgetFocus(dockWidget);
}

void DockFocusController::notifyFloatingWidgetDrop(DockFloatingContainer *floatingWidget)
{
    Q_D(DockFocusController);
    if (!floatingWidget || d->m_window->isRestoringState()) {
        return;
    }

    auto vDockWidget = floatingWidget->property(s_focusedDockWidgetProperty);
    if (!vDockWidget.isValid()) {
        return;
    }

    auto dockWidget = vDockWidget.value<QPointer<DockWidget>>();
    if (dockWidget) {
        dockWidget->dockPanel()->setCurrentDockWidget(dockWidget);
        DockManager::setWidgetFocus(dockWidget);
    }
}

void DockFocusController::setDockWidgetTabFocused(DockTab *tab)
{
    Q_D(DockFocusController);
    auto dockWidget = tab->dockWidget();
    if (dockWidget) {
        d->updateDockWidgetFocus(dockWidget);
    }
}

void DockFocusController::setDockWidgetTabPressed(bool Value)
{
    Q_D(DockFocusController);
    d->m_tabPressed = Value;
}

void DockFocusController::clearDockWidgetFocus(DockWidget *w)
{
    w->clearFocus();
    updateDockWidgetFocusStyle(w, false);
}

void DockFocusController::setDockWidgetFocused(DockWidget *focusedNow)
{
    Q_D(DockFocusController);
    d->updateDockWidgetFocus(focusedNow);
}

void DockFocusController::onApplicationFocusChanged(QWidget *focusedOld, QWidget *focusedNow)
{
    Q_UNUSED(focusedOld);
    Q_D(DockFocusController);

    if (d->m_window->isRestoringState() || d->m_tabPressed) {
        return;
    }

    if (!focusedNow) {
        return;
    }

    DockWidget *dockWidget = qobject_cast<DockWidget *>(focusedNow);
    if (!dockWidget) {
        dockWidget = internal::findParent<DockWidget *>(focusedNow);
    }

    if (!dockWidget || dockWidget->tab()->isHidden()) {
        return;
    }

    d->updateDockWidgetFocus(dockWidget);
}

void DockFocusController::onFocusWindowChanged(QWindow *focusWindow)
{
    Q_D(DockFocusController);
    if (!focusWindow) {
        return;
    }

    auto vDockWidget = focusWindow->property(s_focusedDockWidgetProperty);
    if (!vDockWidget.isValid()) {
        return;
    }

    auto dockWidget = vDockWidget.value<QPointer<DockWidget>>();
    if (!dockWidget) {
        return;
    }

    d->updateDockWidgetFocus(dockWidget);
}

void DockFocusController::onFocusedDockAreaViewToggled(bool open)
{
    Q_D(DockFocusController);

    if (d->m_window->isRestoringState()) {
        return;
    }

    DockPanel *panel = qobject_cast<DockPanel *>(sender());
    if (!panel || open) {
        return;
    }
    auto container = panel->dockContainer();
    auto openedDockPanels = container->openedDockPanels();
    if (openedDockPanels.isEmpty()) {
        return;
    }

    d->updateDockWidgetFocus(openedDockPanels[0]->currentDockWidget());
}

void DockFocusController::onDockWidgetVisibilityChanged(bool visible)
{
    Q_D(DockFocusController);
    auto sender = this->sender();
    auto dockWidget = qobject_cast<DockWidget *>(sender);
    disconnect(sender, SIGNAL(visibilityChanged(bool)), this, SLOT(onDockWidgetVisibilityChanged(bool)));
    if (dockWidget && visible) {
        Q_EMIT d->m_window->focusedDockWidgetChanged(d->m_oldFocusedDockWidget, dockWidget);
    }
}

QX_DOCK_END_NAMESPACE
