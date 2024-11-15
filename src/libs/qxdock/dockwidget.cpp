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
#include "docksidetab.h"
#include "dockautohidecontainer.h"

#include <QPointer>
#include <QBoxLayout>
#include <QAction>
#include <QAbstractScrollArea>
#include <QScrollArea>
#include <QToolBar>

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

    void closeAutoHideDockWidgetsIfNeeded();

    void setupToolBar();
    void setupScrollArea();
    void setToolBarStyleFromDockWindow();
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
    DockWidget::MinimumSizeHintMode m_minimumSizeHintMode = DockWidget::MinimumSizeHintFromDockWidget;
    QWidget *m_widget = nullptr;
    QScrollArea *m_scrollArea = nullptr;
    QPointer<DockSideTab> m_sideTab;
    DockWidget::ToolBarStyleSource m_toolBarStyleSource = DockWidget::ToolBarStyleFromDockWindow;
    Qt::ToolButtonStyle m_toolBarStyleDocked = Qt::ToolButtonIconOnly;
    Qt::ToolButtonStyle m_toolBarStyleFloating = Qt::ToolButtonTextUnderIcon;
    QSize m_toolBarIconSizeDocked = QSize(16, 16);
    QSize m_toolBarIconSizeFloating = QSize(24, 24);
    QToolBar *m_toolBar = nullptr;
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
        DockFloatingContainer *floatingWidget = new DockFloatingContainer(q);
        // We use the size hint of the content widget to provide a good initial size
        floatingWidget->resize(m_widget ? m_widget->sizeHint() : q->sizeHint());
        m_tab->show();
        floatingWidget->show();
    } else {
        m_panel->setCurrentDockWidget(q);
        m_panel->toggleView(true);
        m_tab->show();
        auto splitter = m_panel->parentSplitter();
        while (splitter && !splitter->isVisible() && !m_panel->isAutoHide()) {
            splitter->show();
            splitter = internal::findParent<DockSplitter *>(splitter);
        }

        DockContainer *container = m_panel->dockContainer();
        if (container->isFloating()) {
            DockFloatingContainer *floatingWidget = internal::findParent<DockFloatingContainer *>(container);
            floatingWidget->show();
        }

        // If this widget is pinned and there are no opened dock widgets, unpin the auto hide widget by moving it's
        // contents to parent container While restoring state, opened dock widgets are not valid
        if (container->openedDockWidgets().count() == 0 && m_panel->isAutoHide()) {
            m_panel->autoHideContainer()->moveContentsToParent();
        }
    }
}

void DockWidgetPrivate::hideDockWidget()
{
    m_tab->hide();
    updateParentDockPanel();

    closeAutoHideDockWidgetsIfNeeded();

    if (m_features.testFlag(DockWidget::DeleteContentOnClose)) {
        if (m_scrollArea) {
            m_scrollArea->takeWidget();
            delete m_scrollArea;
            m_scrollArea = nullptr;
        }
        m_widget->deleteLater();
        m_widget = nullptr;
    }
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

void DockWidgetPrivate::closeAutoHideDockWidgetsIfNeeded()
{
    Q_Q(DockWidget);
    auto container = q->dockContainer();
    if (!container) {
        return;
    }

    // If the dock container is the dock manager, or if it is not empty, then we
    // don't need to do anything
    if ((container == q->dockWindow()) || !container->openedDockWidgets().isEmpty()) {
        return;
    }

    for (auto autoHideWidget : container->autoHideWidgets()) {
        auto dockWidget = autoHideWidget->dockWidget();
        if (dockWidget == q) {
            continue;
        }

        dockWidget->toggleView(false);
    }
}

void DockWidgetPrivate::setupToolBar()
{
    Q_Q(DockWidget);
    m_toolBar = new QToolBar(q);
    m_toolBar->setObjectName("dockWidgetToolBar");
    m_layout->insertWidget(0, m_toolBar);
    m_toolBar->setIconSize(QSize(16, 16));
    m_toolBar->toggleViewAction()->setEnabled(false);
    m_toolBar->toggleViewAction()->setVisible(false);
    q->connect(q, SIGNAL(topLevelChanged(bool)), SLOT(setToolbarFloatingStyle(bool)));
}

void DockWidgetPrivate::setupScrollArea()
{
    Q_Q(DockWidget);
    m_scrollArea = new QScrollArea(q);
    m_scrollArea->setObjectName("dockWidgetScrollArea");
    m_scrollArea->setWidgetResizable(true);
    m_layout->addWidget(m_scrollArea);
}

void DockWidgetPrivate::setToolBarStyleFromDockWindow()
{
    Q_Q(DockWidget);
    if (!m_window) {
        return;
    }
    auto state = DockWidget::StateDocked;
    q->setToolBarIconSize(m_window->dockWidgetToolBarIconSize(state), state);
    q->setToolBarStyle(m_window->dockWidgetToolBarStyle(state), state);
    state = DockWidget::StateFloating;
    q->setToolBarIconSize(m_window->dockWidgetToolBarIconSize(state), state);
    q->setToolBarStyle(m_window->dockWidgetToolBarStyle(state), state);
}

DockWidget::DockWidget(const QString &title, QWidget *parent)
    : QWidget{parent}
{
    QX_INIT_PRIVATE(DockWidget);

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

    setAttribute(Qt::WA_StyledBackground, true);
}

DockWidget::~DockWidget()
{
    QX_FINI_PRIVATE();
}

void DockWidget::setWidget(QWidget *w, WidgetInsertMode insertMode)
{
    Q_D(DockWidget);
    if (d->m_widget) {
        takeWidget();
    }

    auto scrollAreaWidget = qobject_cast<QAbstractScrollArea *>(w);
    if (scrollAreaWidget || ForceNoScrollArea == insertMode) {
        d->m_layout->addWidget(w);
        if (scrollAreaWidget && scrollAreaWidget->viewport()) {
            scrollAreaWidget->viewport()->setProperty("dockWidgetContent", true);
        }
    } else {
        d->setupScrollArea();
        d->m_scrollArea->setWidget(w);
    }

    d->m_widget = w;
    d->m_widget->setProperty("dockWidgetContent", true);
}

QWidget *DockWidget::widget() const
{
    Q_D(const DockWidget);
    return d->m_widget;
}

QWidget *DockWidget::takeWidget()
{
    Q_D(DockWidget);
    QWidget *w = nullptr;
    if (d->m_scrollArea) {
        d->m_layout->removeWidget(d->m_scrollArea);
        w = d->m_scrollArea->takeWidget();
        delete d->m_scrollArea;
        d->m_scrollArea = nullptr;
        d->m_widget = nullptr;
    } else if (d->m_widget) {
        d->m_layout->removeWidget(d->m_widget);
        w = d->m_widget;
        d->m_widget = nullptr;
    }

    if (w) {
        w->setParent(nullptr);
    }
    return w;
}

DockTab *DockWidget::tab() const
{
    Q_D(const DockWidget);
    return d->m_tab;
}

DockWidget::DockWidgetFeatures DockWidget::features() const
{
    Q_D(const DockWidget);
    if (d->m_window) {
        return d->m_features & ~d->m_window->globallyLockedDockWidgetFeatures();
    } else {
        return d->m_features;
    }
}

void DockWidget::setFeatures(DockWidgetFeatures features)
{
    Q_D(DockWidget);
    if (d->m_features == features) {
        return;
    }
    d->m_features = features;
    notifyFeaturesChanged();
}

void DockWidget::setFeature(DockWidget::DockWidgetFeature flag, bool on)
{
    auto fs = features();
    internal::setFlag(fs, flag, on);
    setFeatures(fs);
}

void DockWidget::notifyFeaturesChanged()
{
    Q_D(DockWidget);
    Q_EMIT featuresChanged(d->m_features);
    d->m_tab->onDockWidgetFeaturesChanged();
    if (DockPanel *panel = dockPanel()) {
        panel->onDockWidgetFeaturesChanged();
    }
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

DockFloatingContainer *DockWidget::dockFloatingContainer() const
{
    auto container = dockContainer();
    return container ? container->floatingWidget() : nullptr;
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

bool DockWidget::isFullScreen() const
{
    if (isFloating()) {
        return dockContainer()->floatingWidget()->isFullScreen();
    } else {
        return Super::isFullScreen();
    }
}

bool DockWidget::isTabbed() const
{
    Q_D(const DockWidget);
    return d->m_panel && (d->m_panel->openDockWidgetsCount() > 1);
}

bool DockWidget::isCurrentTab() const
{
    Q_D(const DockWidget);
    return d->m_panel && (d->m_panel->currentDockWidget() == this);
}

bool DockWidget::isCentralWidget() const
{
    return dockWindow()->centralWidget() == this;
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

DockWidget::MinimumSizeHintMode DockWidget::minimumSizeHintMode() const
{
    Q_D(const DockWidget);
    return d->m_minimumSizeHintMode;
}

void DockWidget::setMinimumSizeHintMode(DockWidget::MinimumSizeHintMode mode)
{
    Q_D(DockWidget);
    d->m_minimumSizeHintMode = mode;
}

QSize DockWidget::minimumSizeHint() const
{
    Q_D(const DockWidget);
    if (!d->m_widget) {
        return QSize(60, 40);
    }

    switch (d->m_minimumSizeHintMode) {
    case MinimumSizeHintFromDockWidget:
        return QSize(60, 40);
    case MinimumSizeHintFromContent:
        return d->m_widget->minimumSizeHint();
    case MinimumSizeHintFromDockWidgetMinimumSize:
        return minimumSize();
    case MinimumSizeHintFromContentMinimumSize:
        return d->m_widget->minimumSize();
    }

    return d->m_widget->minimumSizeHint();
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

QToolBar *DockWidget::toolBar() const
{
    if (!d_ptr->m_toolBar) {
        d_ptr->setupToolBar();
    }
    return d_ptr->m_toolBar;
}

void DockWidget::setToolBar(QToolBar *toolBar)
{
    Q_D(DockWidget);
    if (d->m_toolBar) {
        delete d->m_toolBar;
    }

    d->m_toolBar = toolBar;
    d->m_layout->insertWidget(0, d->m_toolBar);
    this->connect(this, SIGNAL(topLevelChanged(bool)), SLOT(setToolBarFloatingStyle(bool)));
    setToolBarFloatingStyle(isFloating());
}

DockWidget::ToolBarStyleSource DockWidget::toolBarStyleSource() const
{
    Q_D(const DockWidget);
    return d->m_toolBarStyleSource;
}

void DockWidget::setToolBarStyleSource(DockWidget::ToolBarStyleSource source)
{
    Q_D(DockWidget);
    d->m_toolBarStyleSource = source;
    if (ToolBarStyleFromDockWindow == d->m_toolBarStyleSource) {
        d->setToolBarStyleFromDockWindow();
    }
}

Qt::ToolButtonStyle DockWidget::toolBarStyle(DockWidget::State state) const
{
    Q_D(const DockWidget);
    if (StateFloating == state) {
        return d->m_toolBarStyleFloating;
    } else {
        return d->m_toolBarStyleDocked;
    }
}

void DockWidget::setToolBarStyle(Qt::ToolButtonStyle style, DockWidget::State state)
{
    Q_D(DockWidget);
    if (StateFloating == state) {
        d->m_toolBarStyleFloating = style;
    } else {
        d->m_toolBarStyleDocked = style;
    }

    setToolBarFloatingStyle(isFloating());
}

QSize DockWidget::toolBarIconSize(DockWidget::State state) const
{
    Q_D(const DockWidget);
    if (StateFloating == state) {
        return d->m_toolBarIconSizeFloating;
    } else {
        return d->m_toolBarIconSizeDocked;
    }
}

void DockWidget::setToolBarIconSize(const QSize &iconSize, DockWidget::State state)
{
    Q_D(DockWidget);
    if (StateFloating == state) {
        d->m_toolBarIconSizeFloating = iconSize;
    } else {
        d->m_toolBarIconSizeDocked = iconSize;
    }

    setToolBarFloatingStyle(isFloating());
}

DockSideTab *DockWidget::sideTab() const
{
    Q_D(const DockWidget);
    return d->m_sideTab;
}

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

DockAutoHideContainer *DockWidget::autoHideContainer() const
{
    Q_D(const DockWidget);

    if (!d->m_panel) {
        return nullptr;
    }

    return d->m_panel->autoHideContainer();
}

Qx::DockSideBarArea DockWidget::autoHideArea() const
{
    return isAutoHide() ? autoHideContainer()->sideBarArea() : Qx::DockSideBarNone;
}

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

void DockWidget::deleteDockWidget()
{
    Q_D(DockWidget);
    if (d->m_window) {
        d->m_window->removeDockWidget(this);
    }
    deleteLater();
    d->m_closed = true;
}

void DockWidget::closeDockWidget()
{
    closeDockWidgetInternal(true);
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
        autoHideContainer()->moveToNewSideBarArea(area);
    } else {
        auto location = (Qx::DockSideBarNone == area) ? panel->calculateSideBarArea() : area;
        dockContainer()->createAndSetupAutoHideContainer(location, this, tabIndex);
    }
}

void DockWidget::toggleAutoHide(Qx::DockSideBarArea area)
{
    if (!DockManager::testAutoHideConfigFlag(DockManager::AutoHideFeatureEnabled)) {
        return;
    }

    setAutoHide(!isAutoHide(), area);
}

void DockWidget::setAsCurrentTab()
{
    Q_D(DockWidget);
    if (d->m_panel && !isClosed()) {
        d->m_panel->setCurrentDockWidget(this);
    }
}

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

void DockWidget::showFullScreen()
{
    if (isFloating()) {
        dockContainer()->floatingWidget()->showFullScreen();
    } else {
        Super::showFullScreen();
    }
}

void DockWidget::showNormal()
{
    if (isFloating()) {
        dockContainer()->floatingWidget()->showNormal();
    } else {
        Super::showNormal();
    }
}

void DockWidget::setToolBarFloatingStyle(bool floating)
{
    Q_D(DockWidget);
    if (!d->m_toolBar) {
        return;
    }

    auto iconSize = floating ? d->m_toolBarIconSizeFloating : d->m_toolBarIconSizeDocked;
    if (iconSize != d->m_toolBar->iconSize()) {
        d->m_toolBar->setIconSize(iconSize);
    }

    auto buttonStyle = floating ? d->m_toolBarStyleFloating : d->m_toolBarStyleDocked;
    if (buttonStyle != d->m_toolBar->toolButtonStyle()) {
        d->m_toolBar->setToolButtonStyle(buttonStyle);
    }
}

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
        if (d->m_panel->isAutoHide()) {
            // FIXME: toggleView(true) will show sidTab before mainwindow
            // d->m_panel->autoHideContainer()->toggleView(open);
        }
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

bool DockWidget::event(QEvent *e)
{
    Q_D(DockWidget);
    switch (e->type()) {
    case QEvent::Hide:
        Q_EMIT visibilityChanged(false);
        break;

    case QEvent::Show:
        Q_EMIT visibilityChanged(geometry().right() >= 0 && geometry().bottom() >= 0);
        break;

    case QEvent::WindowTitleChange: {
        const auto title = windowTitle();
        if (d->m_tab) {
            d->m_tab->setText(title);
        }
        if (d->m_sideTab) {
            d->m_sideTab->setText(title);
        }
        if (d->m_toggleViewAction) {
            d->m_toggleViewAction->setText(title);
        }
        if (d->m_panel) {
            d->m_panel->markTitleBarMenuOutdated();   // update tabs menu
        }

        auto floatingWidget = dockFloatingContainer();
        if (floatingWidget) {
            floatingWidget->updateWindowTitle();
        }
        Q_EMIT titleChanged(title);
    } break;

    default:
        break;
    }

    return QWidget::event(e);
}

QX_DOCK_END_NAMESPACE
