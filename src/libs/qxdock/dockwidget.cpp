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
#include "docksplitter.h"
#include "dockfloatingcontainer.h"
#include "docksidetab.h"

#include <QPointer>
#include <QBoxLayout>
#include <QAction>
#include <QAbstractScrollArea>
#include <QScrollArea>
#include <QToolBar>
#include <QXmlStreamWriter>

QX_DOCK_BEGIN_NAMESPACE

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
        // The content can be gone already: takeWidget() is the documented way
        // for a caller to take it back, and showDockWidget() rebuilds it from
        // the factory, so this class runs with m_widget == nullptr.
        if (m_widget) {
            m_widget->deleteLater();
            m_widget = nullptr;
        }
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

    // Accessibility (B2): expose the dock widget title (sans the '&' mnemonic) as
    // the accessible name, so assistive tools can announce the widget; keep it in
    // step with later title changes.
    setAccessibleName(QString(title).remove(QLatin1Char('&')));
    connect(this, &QWidget::windowTitleChanged, this, [this](const QString &t) {
        setAccessibleName(QString(t).remove(QLatin1Char('&')));
    });

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
