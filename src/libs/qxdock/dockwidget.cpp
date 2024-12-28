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
#include <QXmlStreamWriter>

QX_DOCK_BEGIN_NAMESPACE

class DockWidgetPrivate
{
public:
    struct WidgetFactory {
        DockWidget::FactoryFunc createWidget;
        DockWidget::WidgetInsertMode insertMode;
    };

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
    bool createWidgetFromFactory();
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
    WidgetFactory *m_factory = nullptr;
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

    // Allow the widget not to be set when the DeleteContentOnClose flag is not set
    if (!m_widget && m_features.testFlag(DockWidget::DeleteContentOnClose)) {
        if (!createWidgetFromFactory()) {
            Q_ASSERT(m_features.testFlag(DockWidget::DeleteContentOnClose) &&
                     "DeleteContentOnClose flag was set, but the widget "
                     "factory is missing or it doesn't return a valid QWidget.");
            return;
        }
    }

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
        if (container->openedDockWidgets().count() == 0 && m_panel->isAutoHide() && !m_window->isRestoringState()) {
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

/**
 * Hides a dock panel if all dock widgets in the area panel closed.
 * This function updates the current selected tab and hides the parent
 * dock panel if it is empty
 */
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

/**
 * Closes all auto hide dock widgets if there are no more opened dock panels
 * This prevents the auto hide dock widgets from being pinned to an empty dock panel
 */
void DockWidgetPrivate::closeAutoHideDockWidgetsIfNeeded()
{
    Q_Q(DockWidget);
    auto container = q->dockContainer();
    if (!container) {
        return;
    }

    if (q->dockWindow()->isRestoringState()) {
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

/**
 * Setup the main scroll area
 */
void DockWidgetPrivate::setupScrollArea()
{
    Q_Q(DockWidget);
    m_scrollArea = new QScrollArea(q);
    m_scrollArea->setObjectName("dockWidgetScrollArea");
    m_scrollArea->setWidgetResizable(true);
    m_layout->addWidget(m_scrollArea);
}

/**
 * Creates the content widget with the registered widget factory and
 * returns true on success.
 */
bool DockWidgetPrivate::createWidgetFromFactory()
{
    Q_Q(DockWidget);
    if (!m_factory) {
        return false;
    }

    QWidget *w = m_factory->createWidget(q);
    if (!w) {
        return false;
    }

    q->setWidget(w, m_factory->insertMode);
    return true;
}

/**
 * Use the dock window toolbar style and icon size for the different states
 */
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

/**
 * This constructor creates a dock widget with the given title.
 * The title is the text that is shown in the window title when the dock
 * widget is floating and it is the title that is shown in the titlebar
 * or the tab of this dock widget if it is tabified.
 * The object name of the dock widget is also set to the title. The
 * object name is required by the dock window to properly save and restore
 * the state of the dock widget. That means, the title needs to be unique.
 * If your title is not unique or if you would like to change the title
 * during runtime, you need to set a unique object name explicitly
 * by calling setObjectName() after construction.
 * Use the layoutFlags to configure the layout of the dock widget.
 */
DockWidget::DockWidget(const QString &title, QWidget *parent)
    : Super{parent}
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
    setToolBarFloatingStyle(false);

    if (DockManager::testConfigFlag(DockManager::FocusHighlighting)) {
        setFocusPolicy(Qt::ClickFocus);
    }
    QXDOCK_SET_BGCOLOR(this);
}

DockWidget::~DockWidget()
{
    QX_FINI_PRIVATE();
}

/**
 * Sets the widget for the dock widget to widget.
 * \see WidgetInsertMode for a detailed description
 */
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

/**
 * Returns the widget for the dock widget. This function returns zero if
 * the widget has not been set.
 */
QWidget *DockWidget::widget() const
{
    Q_D(const DockWidget);
    return d->m_widget;
}

/**
 * Remove the widget from the dock and give ownership back to the caller
 */
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

/**
 * Returns the tab widget of this dock widget that is shown in the dock
 * title bar
 */
DockTab *DockWidget::tab() const
{
    Q_D(const DockWidget);
    return d->m_tab;
}

/**
 * This property holds whether the dock widget is movable, closable, and
 * floatable.
 * By default, this property is set to a combination of DockWidgetClosable,
 * DockWidgetMovable and DockWidgetFloatable.
 */
DockWidget::DockWidgetFeatures DockWidget::features() const
{
    Q_D(const DockWidget);
    if (d->m_window) {
        return d->m_features & ~d->m_window->globallyLockedDockWidgetFeatures();
    } else {
        return d->m_features;
    }
}

/**
 * Sets, whether the dock widget is movable, closable, and floatable.
 */
void DockWidget::setFeatures(DockWidgetFeatures features)
{
    Q_D(DockWidget);
    if (d->m_features == features) {
        return;
    }
    d->m_features = features;
    notifyFeaturesChanged();
}

/**
 * Sets the feature flag for this dock widget if on is true; otherwise
 * clears the flag.
 */
void DockWidget::setFeature(DockWidget::DockWidgetFeature flag, bool on)
{
    auto fs = features();
    internal::setFlag(fs, flag, on);
    setFeatures(fs);
}

/**
 * Triggers notification of feature change signals and functions
 */
void DockWidget::notifyFeaturesChanged()
{
    Q_D(DockWidget);
    Q_EMIT featuresChanged(d->m_features);
    d->m_tab->onDockWidgetFeaturesChanged();
    if (DockPanel *panel = dockPanel()) {
        panel->onDockWidgetFeaturesChanged();
    }
}

/**
 * Returns the dock window that manages the dock widget or 0 if the widget
 * has not been assigned to any dock window yet
 */
DockWindow *DockWidget::dockWindow() const
{
    Q_D(const DockWidget);
    return d->m_window;
}

/**
 * Returns the dock container widget this dock panel belongs to or 0
 * if this dock widget has not been docked yet
 */
DockContainer *DockWidget::dockContainer() const
{
    Q_D(const DockWidget);
    if (d->m_panel) {
        return d->m_panel->dockContainer();
    } else {
        return nullptr;
    }
}

/**
 * This function return the floating DockContainer if is isFloating() is true
 * and a nullptr if this dock widget is not floating.
 */
DockFloatingContainer *DockWidget::dockFloatingContainer() const
{
    auto container = dockContainer();
    return container ? container->floatingWidget() : nullptr;
}

/**
 * Returns the dock panel this dock widget belongs to or 0
 * if this dock widget has not been docked yet
 */
DockPanel *DockWidget::dockPanel() const
{
    Q_D(const DockWidget);
    return d->m_panel;
}

/**
 * This property holds whether the dock widget is floating.
 * A dock widget is only floating, if it is the one and only widget inside
 * of a floating container. If there are more than one dock widget in a
 * floating container, the all dock widgets are docked and not floating.
 */
bool DockWidget::isFloating() const
{
    if (!isInFloatingContainer()) {
        return false;
    }

    return dockContainer()->topLevelDockWidget() == this;
}

/**
 * The function returns true, if the dock widget is floating and it also
 * returns true if it is docked inside of a floating container.
 */
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

/**
 * Returns true, if this dock widget is closed.
 */
bool DockWidget::isClosed() const
{
    Q_D(const DockWidget);
    return d->m_closed;
}

/**
 * Returns true if the dock widget is floating and if the floating dock
 * container is full screen
 */
bool DockWidget::isFullScreen() const
{
    if (isFloating()) {
        return dockContainer()->floatingWidget()->isFullScreen();
    } else {
        return Super::isFullScreen();
    }
}

/**
 * Returns true if this dock widget is in a dock panel, that contains at
 * least 2 opened dock widgets
 */
bool DockWidget::isTabbed() const
{
    Q_D(const DockWidget);
    return d->m_panel && (d->m_panel->openDockWidgetsCount() > 1);
}

/**
 * Returns true if this dock widget is the current one in the dock
 * panel that contains it.
 * If the dock widget is the only opened dock widget in a dock panel,
 * the true is returned
 */
bool DockWidget::isCurrentTab() const
{
    Q_D(const DockWidget);
    return d->m_panel && (d->m_panel->currentDockWidget() == this);
}

/**
 * Returns true if the dock widget is set as central widget of it's dock window
 */
bool DockWidget::isCentralWidget() const
{
    return dockWindow()->centralWidget() == this;
}

/**
 * Returns a checkable action that can be used to show or close this dock widget.
 * The action's text is set to the dock widget's window title.
 */
QAction *DockWidget::toggleViewAction() const
{
    Q_D(const DockWidget);
    return d->m_toggleViewAction;
}

/**
 * Use provided action to be the default toggle view action for this dock widget.
 * This dock widget now owns the action.
 */
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

/**
 * Configures the behavior of the toggle view action.
 * \see ToggleViewActionMode for a detailed description
 */
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

/**
 * This function changes the toggle view action without emitting any signal
 */
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

/**
 * Set the actions that will be shown in the dock title bar
 * if this dock widget is the active tab.
 * You should not add to many actions to the title bar, because this
 * will remove the available space for the tabs. If you have a number
 * of actions, just add an action with a menu to show a popup menu
 * button in the title bar.
 */
void DockWidget::setTitleBarActions(QList<QAction *> actions)
{
    Q_D(DockWidget);
    d->m_titleBarActions = actions;
}

/**
 * Get the minimum size hint mode configured by setMinimumSizeHintMode
 */
DockWidget::MinimumSizeHintMode DockWidget::minimumSizeHintMode() const
{
    Q_D(const DockWidget);
    return d->m_minimumSizeHintMode;
}

/**
 * Configures the minimum size hint that is returned by the
 * minimumSizeHint() function.
 * \see MinimumSizeHintMode for a detailed description
 */
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

    if (d->m_sideTab) {
        d->m_sideTab->setIcon(icon);
    }

    if (!d->m_toggleViewAction->isCheckable()) {
        d->m_toggleViewAction->setIcon(icon);
    }
}

/**
 * This function returns the dock widget top tool bar.
 * a default empty toolbar is assigned or you need to assign your custom
 * toolbar via setToolBar().
 */
QToolBar *DockWidget::toolBar() const
{
    if (!d_ptr->m_toolBar) {
        d_ptr->setupToolBar();
    }
    return d_ptr->m_toolBar;
}

/**
 * Assign a new tool bar that is shown above the content widget.
 * The dock widget will become the owner of the tool bar and deletes it
 * on destruction
 */
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

/**
 * Configures, if the dock widget uses the global tool bar styles from
 * dock window or if it uses its own tool bar style
 */
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

/**
 * This function sets the tool button style for the given dock widget state.
 * It is possible to switch the tool button style depending on the state.
 * If a dock widget is floating, then here are more space and it is
 * possible to select a style that requires more space like
 * Qt::ToolButtonTextUnderIcon. For the docked state Qt::ToolButtonIconOnly
 * might be better.
 */
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

/**
 * This function sets the tool button icon size for the given state.
 * If a dock widget is floating, there is more space an increasing the
 * icon size is possible. For docked widgets, small icon sizes, eg. 16 x 16
 * might be better.
 */
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
        autoHideContainer()->moveToNewSideBarArea(area);
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
 * Adjusts the toolbar icon sizes according to the floating state
 */
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

void DockWidget::saveState(QXmlStreamWriter &s) const
{
    Q_D(const DockWidget);
    s.writeStartElement("Widget");
    s.writeAttribute("Name", objectName());
    s.writeAttribute("Closed", QString::number(d->m_closed ? 1 : 0));
    s.writeEndElement();
}

/**
 * Emits titleChanged signal if title change event occurs
 */
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

    return Super::event(e);
}

QX_DOCK_END_NAMESPACE
