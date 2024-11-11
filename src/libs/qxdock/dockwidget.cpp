/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockwidget.h"
#include "dockwindow.h"
#include "dockpanel.h"
#include "docktab.h"
#include "dockmanager.h"
#include "dockutils.h"
#include "docksplitter.h"
#include "dockfloatingcontainer.h"

#include <QPointer>
#include <QBoxLayout>
#include <QAction>

QX_DOCK_BEGIN_NAMESPACE

class DockWidgetPrivate
{
public:
    QX_DECLARE_PUBLIC(DockWidget)
public:
    DockWidgetPrivate();
    void init();
    void showDockWidget();
    void hideDockWidget();
    void updateParentDockPanel();
public:
    DockWindow *m_window = nullptr;
    QPointer<DockPanel> m_panel = nullptr;
    QBoxLayout *m_layout = nullptr;
    DockTab *m_tab = nullptr;
    DockWidget::DockWidgetFeatures m_features = DockWidget::DefaultDockWidgetFeatures;
    QList<QAction *> m_titleBarActions;
    QAction *m_toggleViewAction = nullptr;
    bool m_closed = false;
    bool m_isFloatingTopLevel = false;
};

DockWidgetPrivate::DockWidgetPrivate()
{
}

void DockWidgetPrivate::init()
{
    Q_Q(DockWidget);

    m_layout = new QBoxLayout(QBoxLayout::TopToBottom);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    q->setLayout(m_layout);

    m_tab = new DockTab(q);
}

void DockWidgetPrivate::showDockWidget()
{
    Q_Q(DockWidget);

    if (!m_panel) {
    } else {
        m_panel->setCurrentDockWidget(q);
        m_panel->toggleView(true);
        m_tab->show();
        auto splitter = m_panel->parentSplitter();
        while (splitter && !splitter->isVisible()) {
            splitter->show();
            splitter = internal::findParent<DockSplitter *>(splitter);
        }

        DockContainer *container = m_panel->dockContainer();
        if (container->isFloating()) {
            DockFloatingContainer *floatingWidget = internal::findParent<DockFloatingContainer *>(container);
            floatingWidget->show();
        }
    }
}

void DockWidgetPrivate::hideDockWidget()
{
    m_tab->hide();
    updateParentDockPanel();
}

void DockWidgetPrivate::updateParentDockPanel()
{
    if (!m_panel) {
        return;
    }
    Q_Q(DockWidget);
    if (m_panel->currentDockWidget() != q) {
        return;
    }
    auto nextDockWidget = m_panel->nextOpenDockWidget(q);
    if (nextDockWidget) {
        m_panel->setCurrentDockWidget(nextDockWidget);
    } else {
        m_panel->hideAreaWithNoVisibleContent();
    }
}

DockWidget::DockWidget(const QString &title, QWidget *parent)
    : QWidget{parent}
{
    QX_INIT_PRIVATE(DockWidget)

    // must before d->init(), create tab need title
    setWindowTitle(title);
    setObjectName(title);

    Q_D(DockWidget);
    d->init();

    d->m_toggleViewAction = new QAction(title, this);
    d->m_toggleViewAction->setCheckable(true);
    connect(d->m_toggleViewAction, SIGNAL(triggered(bool)), this, SLOT(toggleView(bool)));

    if (DockManager::testConfigFlag(DockManager::FocusHighlighting)) {
        setFocusPolicy(Qt::ClickFocus);
    }
}

DockWidget::~DockWidget(){QX_FINI_PRIVATE()}

DockTab *DockWidget::tab() const
{
    Q_D(const DockWidget);
    return d->m_tab;
}

DockWidget::DockWidgetFeatures DockWidget::features() const
{
    Q_D(const DockWidget);
    return d->m_features;
}

DockWindow *DockWidget::dockWindow() const
{
    Q_D(const DockWidget);
    return d->m_window;
}

DockContainer *DockWidget::dockContainer() const
{
    Q_D(const DockWidget);
    if (d->m_panel) {
        return d->m_panel->dockContainer();
    } else {
        return nullptr;
    }
}

DockPanel *DockWidget::dockPanel() const
{
    Q_D(const DockWidget);
    return d->m_panel;
}

bool DockWidget::isFloating() const
{
    if (!isInFloatingContainer()) {
        return false;
    }

    return dockContainer()->topLevelDockWidget() == this;
}

bool DockWidget::isInFloatingContainer() const
{
    auto container = dockContainer();
    if (!container) {
        return false;
    }

    if (!container->isFloating()) {
        return false;
    }

    return true;
}

bool DockWidget::isClosed() const
{
    Q_D(const DockWidget);
    return d->m_closed;
}

QAction *DockWidget::toggleViewAction() const
{
    Q_D(const DockWidget);
    return d->m_toggleViewAction;
}

void DockWidget::setToggleViewAction(QAction *action)
{
    if (!action) {
        return;
    }
    Q_D(DockWidget);
    d->m_toggleViewAction->setParent(nullptr);
    delete d->m_toggleViewAction;
    d->m_toggleViewAction = action;
    d->m_toggleViewAction->setParent(this);
    connect(d->m_toggleViewAction, SIGNAL(triggered(bool)), this, SLOT(toggleView(bool)));
}

void DockWidget::setToggleViewActionMode(DockWidget::ToggleViewActionMode mode)
{
    Q_D(DockWidget);
    if (mode == ActionModeToggle) {
        d->m_toggleViewAction->setCheckable(true);
        d->m_toggleViewAction->setIcon(QIcon());
    } else {
        d->m_toggleViewAction->setCheckable(false);
        d->m_toggleViewAction->setIcon(d->m_tab->icon());
    }
}

void DockWidget::setToggleViewActionChecked(bool checked)
{
    Q_D(DockWidget);
    QAction *action = d->m_toggleViewAction;
    action->blockSignals(true);
    action->setChecked(checked);
    action->blockSignals(false);
}

QList<QAction *> DockWidget::titleBarActions() const
{
    Q_D(const DockWidget);
    return d->m_titleBarActions;
}

QIcon DockWidget::icon() const
{
    Q_D(const DockWidget);
    return d->m_tab->icon();
}

void DockWidget::setIcon(const QIcon &icon)
{
    Q_D(DockWidget);
    d->m_tab->setIcon(icon);

    if (!d->m_toggleViewAction->isCheckable()) {
        d->m_toggleViewAction->setIcon(icon);
    }
}

void DockWidget::toggleView(bool open)
{
    Q_D(DockWidget);
    QAction *a = qobject_cast<QAction *>(sender());
    if (a == d->m_toggleViewAction && !d->m_toggleViewAction->isCheckable()) {
        open = true;
    }
    if (d->m_closed != !open) {
        toggleViewInternal(open);
    }
}

void DockWidget::deleteDockWidget()
{
    Q_D(DockWidget);
    if (d->m_window) {
        d->m_window->removeDockWidget(this);
    }
}

void DockWidget::requestCloseDockWidget()
{
    if (features().testFlag(DockWidget::DockWidgetDeleteOnClose) ||
        features().testFlag(DockWidget::CustomCloseHandling)) {
        closeDockWidgetInternal(false);
    } else {
        toggleView(false);
    }
}

void DockWidget::setDockWindow(DockWindow *window)
{
    Q_D(DockWidget);
    d->m_window = window;
}

void DockWidget::setDockPanel(DockPanel *panel)
{
    Q_D(DockWidget);
    d->m_panel = panel;
    setParent(panel);
}

void DockWidget::setClosedState(bool closed)
{
    Q_D(DockWidget);
    d->m_closed = closed;
}

void DockWidget::emitTopLevelEventForWidget(DockWidget *topLevelDockWidget, bool floating)
{
    if (topLevelDockWidget) {
        topLevelDockWidget->dockPanel()->updateTitleBarVisibility();
        topLevelDockWidget->emitTopLevelChanged(floating);
    }
}

void DockWidget::emitTopLevelChanged(bool floating)
{
    Q_D(DockWidget);
    if (floating != d->m_isFloatingTopLevel) {
        d->m_isFloatingTopLevel = floating;
        Q_EMIT topLevelChanged(floating);
    }
}

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
        deleteDockWidget();
        Q_EMIT closed();
    } else {
        toggleView(false);
    }
    return true;
}

QX_DOCK_END_NAMESPACE
