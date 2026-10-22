/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "dockpanel_p.h"

QX_DOCK_BEGIN_NAMESPACE

DockPanelPrivate::DockPanelPrivate()
{
}

DockPanelPrivate::~DockPanelPrivate()
{
    delete m_contentsLayout;
}

void DockPanelPrivate::init()
{
    Q_Q(DockPanel);

    m_layout = new QBoxLayout(QBoxLayout::TopToBottom);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    q->setLayout(m_layout);

    m_titleBar = new DockTitleBar(q);
    QObject::connect(m_titleBar, &DockTitleBar::tabBarClicked, q, &DockPanel::setCurrentIndex);
    QObject::connect(tabBar(), &DockTabBar::tabCloseRequested, q, &DockPanel::onTabCloseRequested);
    QObject::connect(tabBar(), &DockTabBar::tabMoved, q, &DockPanel::reorderDockWidget);
    m_layout->addWidget(m_titleBar);

    m_contentsLayout = new DockAreaLayout(m_layout);
}

DockTabBar *DockPanelPrivate::tabBar() const
{
    return m_titleBar->tabBar();
}

void DockPanelPrivate::updateTitleBarButtonStates()
{
    Q_Q(DockPanel);
    if (q->isHidden()) {
        m_updateTitleBarButtons = true;
        return;
    }
    if (q->isAutoHide()) {
        if (DockManager::testAutoHideConfigFlag(DockManager::AutoHideHasCloseButton)) {
            m_titleBar->button(Qx::TitleBarButtonClose)
                ->setEnabled(q->features().testFlag(DockWidget::DockWidgetClosable));
        }
    } else {
        m_titleBar->button(Qx::TitleBarButtonUndock)
            ->setEnabled(q->features().testFlag(DockWidget::DockWidgetFloatable));
        m_titleBar->button(Qx::TitleBarButtonClose)->setEnabled(q->features().testFlag(DockWidget::DockWidgetClosable));
    }

    m_titleBar->button(Qx::TitleBarButtonAutoHide)->setEnabled(q->features().testFlag(DockWidget::DockWidgetPinnable));
    m_titleBar->updateDockWidgetActionsButtons();
    m_updateTitleBarButtons = false;
}

void DockPanelPrivate::updateTitleBarButtonVisibility(bool isTopLevel)
{
    Q_Q(DockPanel);
    auto container = q->dockContainer();
    if (!container) {
        return;
    }
    bool isAutoHide = q->isAutoHide();
    if (isAutoHide) {
        bool showCloseButton = DockManager::testAutoHideConfigFlag(DockManager::AutoHideHasCloseButton);
        m_titleBar->button(Qx::TitleBarButtonClose)->setVisible(showCloseButton);
        m_titleBar->button(Qx::TitleBarButtonAutoHide)->setVisible(true);
        m_titleBar->button(Qx::TitleBarButtonUndock)->setVisible(false);
        m_titleBar->button(Qx::TitleBarButtonTabsMenu)->setVisible(false);
    } else if (isTopLevel) {
        m_titleBar->button(Qx::TitleBarButtonClose)->setVisible(!container->isFloating());
        m_titleBar->button(Qx::TitleBarButtonAutoHide)->setVisible(!container->isFloating());
        // Undock and tabs should never show when auto hidden
        m_titleBar->button(Qx::TitleBarButtonUndock)->setVisible(!container->isFloating());
        m_titleBar->button(Qx::TitleBarButtonTabsMenu)->setVisible(true);
    } else {
        m_titleBar->button(Qx::TitleBarButtonClose)->setVisible(true);
        bool showAutoHideButton = DockManager::testAutoHideConfigFlag(DockManager::DockAreaHasAutoHideButton);
        m_titleBar->button(Qx::TitleBarButtonAutoHide)->setVisible(showAutoHideButton);
        m_titleBar->button(Qx::TitleBarButtonUndock)->setVisible(true);
        m_titleBar->button(Qx::TitleBarButtonTabsMenu)->setVisible(true);
    }
}

/**
 * Scans all contained dock widgets for the max. minimum size hint
 */
void DockPanelPrivate::updateMinimumSizeHint()
{
    m_minSizeHint = QSize();
    for (int i = 0; i < m_contentsLayout->count(); ++i) {
        auto widget = m_contentsLayout->widget(i);
        m_minSizeHint.setHeight(qMax(m_minSizeHint.height(), widget->minimumSizeHint().height()));
        m_minSizeHint.setWidth(qMax(m_minSizeHint.width(), widget->minimumSizeHint().width()));
    }
}

DockPanel::DockPanel(DockWindow *window, DockContainer *parent)
    : Super{parent}
{
    QX_INIT_PRIVATE(DockPanel);

    Q_D(DockPanel);
    d->m_window = window;
    d->init();
    QXDOCK_SET_BGCOLOR(this);
}

DockPanel::~DockPanel()
{
    QX_FINI_PRIVATE();
}

DockWindow *DockPanel::dockWindow() const
{
    Q_D(const DockPanel);
    return d->m_window;
}

DockContainer *DockPanel::dockContainer() const
{
    // need include header of DockContainer
    return internal::findParent<DockContainer *>(this);
}

DockSplitter *DockPanel::parentSplitter() const
{
    // need include header of DockSplitter
    return internal::findParent<DockSplitter *>(this);
}

QRect DockPanel::titleBarGeometry() const
{
    Q_D(const DockPanel);
    return d->m_titleBar->geometry();
}

QRect DockPanel::contentAreaGeometry() const
{
    Q_D(const DockPanel);
    return d->m_contentsLayout->geometry();
}

int DockPanel::dockWidgetsCount() const
{
    Q_D(const DockPanel);
    return d->m_contentsLayout->count();
}

QList<DockWidget *> DockPanel::dockWidgets() const
{
    Q_D(const DockPanel);
    QList<DockWidget *> dwList;
    for (int i = 0; i < d->m_contentsLayout->count(); ++i) {
        dwList.append(dockWidget(i));
    }
    return dwList;
}

int DockPanel::openDockWidgetsCount() const
{
    Q_D(const DockPanel);
    int cnt = 0;
    for (int i = 0; i < d->m_contentsLayout->count(); ++i) {
        DockWidget *w = dockWidget(i);
        if (w && !w->isClosed()) {
            ++cnt;
        }
    }
    return cnt;
}

QList<DockWidget *> DockPanel::openedDockWidgets() const
{
    Q_D(const DockPanel);
    QList<DockWidget *> dwList;
    for (int i = 0; i < d->m_contentsLayout->count(); ++i) {
        DockWidget *w = dockWidget(i);
        if (w && !w->isClosed()) {
            dwList.append(w);
        }
    }
    return dwList;
}

DockWidget *DockPanel::dockWidget(int index) const
{
    Q_D(const DockPanel);
    return qobject_cast<DockWidget *>(d->m_contentsLayout->widget(index));
}

/**
 * Returns the index of the current active dock widget or -1 if there
 * are is no active dock widget (ie.e if all dock widgets are closed)
 */
int DockPanel::currentIndex() const
{
    Q_D(const DockPanel);
    return d->m_contentsLayout->currentIndex();
}

DockWidget *DockPanel::currentDockWidget() const
{
    int index = currentIndex();
    if (index < 0) {
        return nullptr;
    }
    return dockWidget(index);
}

void DockPanel::setCurrentDockWidget(DockWidget *w)
{
    if (dockWindow()->isRestoringState()) {
        return;
    }
    internalSetCurrentDockWidget(w);
}

/**
 * This functions returns the dock widget features of all dock widget in
 * this panel.
 * A bitwise and is used to combine the flags of all dock widgets. That
 * means, if only one single dock widget does not support a certain flag,
 * the whole dock are does not support the flag. I.e. if one single
 * dock widget in this panel is not closable, the whole dock are is not
 * closable.
 */
DockWidget::DockWidgetFeatures DockPanel::features(Qx::DockBitwiseOperator mode) const
{
    if (mode == Qx::DockBitwiseAnd) {
        DockWidget::DockWidgetFeatures f(DockWidget::AllDockWidgetFeatures);
        for (const auto w : dockWidgets()) {
            f &= w->features();
        }
        return f;
    } else {
        DockWidget::DockWidgetFeatures f(DockWidget::NoDockWidgetFeatures);
        for (const auto w : dockWidgets()) {
            f |= w->features();
        }
        return f;
    }
}

QAbstractButton *DockPanel::titleBarButton(Qx::DockTitleBarButton which) const
{
    Q_D(const DockPanel);
    return d->m_titleBar->button(which);
}

void DockPanel::setVisible(bool visible)
{
    Super::setVisible(visible);
    Q_D(DockPanel);
    if (d->m_updateTitleBarButtons) {
        d->updateTitleBarButtonStates();
    }
}

Qx::DockWidgetAreas DockPanel::allowedAreas() const
{
    Q_D(const DockPanel);
    return d->m_allowedAreas;
}

void DockPanel::setAllowedAreas(Qx::DockWidgetAreas areas)
{
    Q_D(DockPanel);
    d->m_allowedAreas = areas;
}

DockTitleBar *DockPanel::titleBar() const
{
    Q_D(const DockPanel);
    return d->m_titleBar;
}

/**
 * Returns the dock area flags - a combination of flags that configure the
 * appearance and features of the dock area.
 * \see setDockAreaFlasg()
 */
DockPanel::DockAreaFlags DockPanel::dockAreaFlags() const
{
    Q_D(const DockPanel);
    return d->m_flags;
}

/**
 * Sets the dock area flags - a combination of flags that configure the
 * appearance and features of the dock panel
 */
void DockPanel::setDockAreaFlags(DockAreaFlags flags)
{
    Q_D(DockPanel);
    auto changedFlags = d->m_flags ^ flags;
    d->m_flags = flags;
    if (changedFlags.testFlag(HideSingleWidgetTitleBar)) {
        updateTitleBarVisibility();
    }
}

void DockPanel::setDockAreaFlag(DockPanel::DockAreaFlag flag, bool on)
{
    auto flags = dockAreaFlags();
    internal::setFlag(flags, flag, on);
    setDockAreaFlags(flags);
}

bool DockPanel::isCentralWidgetArea() const
{
    if (dockWidgetsCount() != 1) {
        return false;
    }

    return dockWindow()->centralWidget() == dockWidgets().constFirst();
}

bool DockPanel::containsCentralWidget() const
{
    auto centralWidget = dockWindow()->centralWidget();
    for (const auto &dockWidget : dockWidgets()) {
        if (dockWidget == centralWidget) {
            return true;
        }
    }

    return false;
}

bool DockPanel::isTopLevelArea() const
{
    auto container = dockContainer();
    if (!container) {
        return false;
    }

    return (container->topLevelDockPanel() == this);
}

void DockPanel::setCurrentIndex(int index)
{
    Q_D(DockPanel);
    auto tabBar = d->tabBar();
    if (index < 0 || index > (tabBar->count() - 1)) {
        qWarning() << Q_FUNC_INFO << "Invalid index" << index;
        return;
    }

    auto cw = d->m_contentsLayout->currentWidget();
    auto nw = d->m_contentsLayout->widget(index);
    if (cw == nw && !nw->isHidden()) {
        return;
    }

    Q_EMIT currentChanging(index);
    tabBar->setCurrentIndex(index);
    d->m_contentsLayout->setCurrentIndex(index);
    d->m_contentsLayout->currentWidget()->show();
    Q_EMIT currentChanged(index);
}

/**
 * Add a new dock widget to dock panel.
 * All dockwidgets in the dock panel tabified in a stacked layout with tabs
 */
void DockPanel::addDockWidget(DockWidget *w)
{
    Q_D(DockPanel);
    insertDockWidget(d->m_contentsLayout->count(), w);
}

/**
 * Inserts a dock widget into dock panel.
 * All dockwidgets in the dock panel tabified in a stacked layout with tabs.
 * The index indicates the index of the new dockwidget in the tabbar and
 * in the stacked layout. If the activate parameter is true, the new
 * w will be the active one in the stacked layout
 */
void DockPanel::insertDockWidget(int index, DockWidget *w, bool activate)
{
    Q_D(DockPanel);
    if (index < 0 || index > d->m_contentsLayout->count()) {
        index = d->m_contentsLayout->count();
    }
    d->m_contentsLayout->insertWidget(index, w);
    w->setDockPanel(this);
    auto tab = w->tab();
    tab->setDockPanel(this);
    d->tabBar()->blockSignals(true);
    d->tabBar()->insertTab(index, tab);
    d->tabBar()->blockSignals(false);
    tab->setVisible(!w->isClosed());
    d->m_titleBar->autoHideTitleLabel()->setText(w->windowTitle());
    w->setProperty(INDEX_PROPERTY, index);
    d->m_minSizeHint.setHeight(qMax(d->m_minSizeHint.height(), w->minimumSizeHint().height()));
    d->m_minSizeHint.setWidth(qMax(d->m_minSizeHint.width(), w->minimumSizeHint().width()));
    if (activate) {
        setCurrentIndex(index);
        // Set current index can show the widget without changing the close state,
        // added to keep the close state consistent
        w->setClosedState(false);
    }
    // If this dock area is hidden because all of its dock widgets were closed,
    // then we need to make it visible again by calling
    // DockWidget->toggleViewInternal(true).
    //
    // An auto hide panel is not in that state - it is hidden by design, which is
    // what auto hide means. Toggling this one open is read by
    // DockWidgetPrivate::showDockWidget() as "the widget is pinned and there is
    // nothing else open, so unpin it", and that immediately undoes the auto hide
    // container we are in the middle of filling: the container is deleted, the
    // widget is docked like any other one, and the panel it had created stays
    // behind empty in the root splitter. It only takes a window whose only dock
    // widgets are auto hidden to get there. The panel is created and its auto
    // hide container assigned before the first widget goes in (see
    // DockAutoHideContainer's constructor), so isAutoHide() is already true.
    if (!this->isVisible() && !isAutoHide() && d->m_contentsLayout->count() > 0 && !dockWindow()->isRestoringState()) {
        // Notes: If d->m_contentsLayout->count() > 1 is required, it will appear
        // that the toggleViewAction is not checked when there is only one dock widget
        w->toggleViewInternal(true);
    }
    d->updateTitleBarButtonStates();
    updateTitleBarVisibility();
}

void DockPanel::removeDockWidget(DockWidget *w)
{
    if (!w) {
        return;
    }
    // If this dock panel is in a auto hide container, then we can delete
    // the auto hide container now
    if (isAutoHide()) {
        autoHideContainer()->cleanupAndDelete();
        return;
    }

    auto currentWidget = currentDockWidget();
    auto nextOpenWidget = (w == currentWidget) ? nextOpenDockWidget(w) : nullptr;

    Q_D(DockPanel);
    d->m_contentsLayout->removeWidget(w);
    auto tab = w->tab();
    tab->hide();
    d->tabBar()->removeTab(tab);
    tab->setParent(w);
    w->setDockPanel(nullptr);

    DockContainer *container = dockContainer();
    if (nextOpenWidget) {
        setCurrentDockWidget(nextOpenWidget);
    } else if (d->m_contentsLayout->isEmpty() && container->dockPanelCount() >= 1) {
        container->removeDockPanel(this);
        this->deleteLater();
        if (container->dockPanelCount() == 0) {
            if (DockFloatingContainer *floatingContainer = container->floatingWidget()) {
                floatingContainer->hide();
                floatingContainer->deleteLater();
            }
        }
    } else if (w == currentWidget) {
        hideAreaWithNoVisibleContent();
    }
    d->updateTitleBarButtonStates();
    updateTitleBarVisibility();
    d->updateMinimumSizeHint();
    auto topLevelWidget = container->topLevelDockWidget();
    if (topLevelWidget) {
        topLevelWidget->emitTopLevelChanged(true);
    }
}

/**
 * This is a helper function to get the next open dock widget to activate
 * if the given w will be closed or removed.
 * The function returns the next widget that should be activated or
 * nullptr in case there are no more open widgets in this area.
 */
DockWidget *DockPanel::nextOpenDockWidget(DockWidget *w) const
{
    auto openWidgets = openedDockWidgets();

    if (openWidgets.count() > 1 || (openWidgets.count() == 1 && openWidgets[0] != w)) {
        if (openWidgets.last() == w) {
            DockWidget *nextDockWidget = openWidgets[openWidgets.count() - 2];
            // search backwards for widget with tab
            for (int i = openWidgets.count() - 2; i >= 0; --i) {
                auto dw = openWidgets[i];
                if (!dw->features().testFlag(DockWidget::NoTab)) {
                    return dw;
                }
            }
            // return widget without tab
            return nextDockWidget;
        } else {
            int indexOfDockWidget = openWidgets.indexOf(w);
            DockWidget *nextDockWidget = openWidgets[indexOfDockWidget + 1];
            // search forwards for widget with tab
            for (int i = indexOfDockWidget + 1; i < openWidgets.count(); ++i) {
                auto dw = openWidgets[i];
                if (!dw->features().testFlag(DockWidget::NoTab)) {
                    return dw;
                }
            }

            // search backwards for widget with tab
            for (int i = indexOfDockWidget - 1; i >= 0; --i) {
                auto dw = openWidgets[i];
                if (!dw->features().testFlag(DockWidget::NoTab)) {
                    return dw;
                }
            }

            // return widget without tab
            return nextDockWidget;
        }
    } else {
        return nullptr;
    }
}

/**
 * Called from dock widget if it is opened or closed
 */
void DockPanel::toggleDockWidgetView(DockWidget *w, bool open)
{
    Q_UNUSED(w);
    Q_UNUSED(open);
    updateTitleBarVisibility();
}

int DockPanel::indexOf(DockWidget *w) const
{
    Q_D(const DockPanel);
    return d->m_contentsLayout->indexOf(w);
}

#ifdef Q_OS_WIN
/**
 * Reimplements QWidget::event to handle QEvent::PlatformSurface
 * This is here to fix issue #294 Tab refresh problem with a QGLWidget
 * that exists since Qt version 5.12.7. So this function is here to
 * work around a Qt issue.
 */
bool DockPanel::event(QEvent *e)
{
    switch (e->type()) {
    case QEvent::PlatformSurface:
        return true;
    default:
        break;
    }

    return Super::event(e);
}
#endif

QX_DOCK_END_NAMESPACE
