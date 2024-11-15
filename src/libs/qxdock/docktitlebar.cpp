/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "docktitlebar.h"
#include "docktitlebar_p.h"
#include "dockpanel.h"
#include "docktabbar.h"
#include "docktab.h"
#include "dockutils.h"
#include "dockmanager.h"
#include "dockfloatingcontainer.h"
#include "dockfloatingpreview.h"
#include "dockcontainer.h"
#include "dockwindow.h"
#include "dockoverlay.h"
#include "dockfocuscontroller.h"
#include "docklabel.h"
#include "dockautohidecontainer.h"

#include <QBoxLayout>
#include <QMenu>
#include <QMouseEvent>
#include <QApplication>

QX_DOCK_BEGIN_NAMESPACE

TitleBarButton::TitleBarButton(bool showInTitleBar, bool hideWhenDisabled, Qx::DockTitleBarButton id, QWidget *parent)
    : QToolButton(parent)
    , m_id(id)
    , m_showInTitleBar(showInTitleBar)
    , m_hideWhenDisabled(hideWhenDisabled)
{
    setFocusPolicy(Qt::NoFocus);
}

void TitleBarButton::setShowInTitleBar(bool show)
{
    this->m_showInTitleBar = show;
    if (!show) {
        setVisible(false);
    }
}

SpacerWidget::SpacerWidget(QWidget *parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setStyleSheet("border: none; background: none;");
}

class DockTitleBarPrivate
{
public:
    QX_DECLARE_PUBLIC(DockTitleBar)
public:
    DockTitleBarPrivate();

    void init();
    void createTabBar();
    void createButtons();
    void createAutoHideTitleLabel();

    DockWindow *dockWindow() const;

    bool isDraggingState(Qx::DockDragState dragState) const;
    void startFloating(const QPoint &offset);
    DockFloatingWidget *makeAreaFloating(const QPoint &offset, Qx::DockDragState dragState);

    static bool testConfigFlag(DockManager::ConfigFlag flag);
    static bool testAutoHideConfigFlag(DockManager::AutoHideFlag flag);
    QAction *createAutoHideToAction(const QString &title, Qx::DockSideBarArea area, QMenu *menu);
public:
    DockPanel *m_panel = nullptr;
    QBoxLayout *m_layout = nullptr;
    DockTabBar *m_tabBar = nullptr;

    QPointer<TitleBarButton> m_tabsMenuButton;
    QPointer<TitleBarButton> m_undockButton;
    QPointer<TitleBarButton> m_closeButton;
    QPointer<TitleBarButton> m_autoHideButton;
    QPointer<TitleBarButton> m_minimizeButton;

    QList<TitleBarButton *> m_dockWidgetActionsButtons;
    bool m_menuOutdated = true;

    Qx::DockDragState m_dragState = Qx::DockDraggingInactive;
    QPoint m_dragStartMousePos;
    DockFloatingWidget *m_floatingWidget = nullptr;
    DockLabel *m_autoHideTitleLabel = nullptr;
};

DockTitleBarPrivate::DockTitleBarPrivate()
{
}

void DockTitleBarPrivate::init()
{
    Q_Q(DockTitleBar);

    m_layout = new QBoxLayout(QBoxLayout::LeftToRight);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    q->setLayout(m_layout);

    createTabBar();
    createButtons();
    createAutoHideTitleLabel();
}

void DockTitleBarPrivate::createTabBar()
{
    Q_Q(DockTitleBar);
    m_tabBar = new DockTabBar(m_panel);
    // The horizontal direction uses Expanding instead of Maximum to maximize expansion
    // I don't know why QSizePolicy::Maximum doesn't work good
    m_tabBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_layout->addWidget(m_tabBar);
    q->connect(m_tabBar, SIGNAL(tabMoved(int, int)), SLOT(markTabsMenuOutdated()));
    q->connect(m_tabBar, SIGNAL(removingTab(int)), SLOT(markTabsMenuOutdated()));
    q->connect(m_tabBar, SIGNAL(currentChanged(int)), SLOT(onCurrentTabChanged(int)));
    q->connect(m_tabBar, SIGNAL(tabBarClicked(int)), SIGNAL(tabBarClicked(int)));
    q->connect(m_tabBar, SIGNAL(elidedChanged(bool)), SLOT(markTabsMenuOutdated()));
}

void DockTitleBarPrivate::createButtons()
{
    Q_Q(DockTitleBar);

    QSizePolicy sp(QSizePolicy::Fixed, QSizePolicy::Expanding);

    // Tabs menu button
    m_tabsMenuButton =
        new TitleBarButton(testConfigFlag(DockManager::DockAreaHasTabsMenuButton), false, Qx::TitleBarButtonTabsMenu);
    m_tabsMenuButton->setObjectName("tabsMenuButton");
    m_tabsMenuButton->setAutoRaise(true);
    m_tabsMenuButton->setPopupMode(QToolButton::InstantPopup);
    internal::setButtonIcon(m_tabsMenuButton, QStyle::SP_TitleBarUnshadeButton, Qx::DockPanelMenuIcon);
    QMenu *tabsMenu = new QMenu(m_tabsMenuButton);
#ifndef QT_NO_TOOLTIP
    tabsMenu->setToolTipsVisible(true);
#endif
    q->connect(tabsMenu, SIGNAL(aboutToShow()), SLOT(onTabsMenuAboutToShow()));
    q->connect(tabsMenu, SIGNAL(triggered(QAction *)), SLOT(onTabsMenuActionTriggered(QAction *)));
    m_tabsMenuButton->setMenu(tabsMenu);
    internal::setToolTip(m_tabsMenuButton, QObject::tr("List All Tabs"));
    m_tabsMenuButton->setSizePolicy(sp);
    m_layout->addWidget(m_tabsMenuButton, 0);

    // Undock button
    m_undockButton =
        new TitleBarButton(testConfigFlag(DockManager::DockAreaHasUndockButton), true, Qx::TitleBarButtonUndock);
    m_undockButton->setObjectName("detachGroupButton");
    m_undockButton->setAutoRaise(true);
    internal::setToolTip(m_undockButton, QObject::tr("Detach Group"));
    internal::setButtonIcon(m_undockButton, QStyle::SP_TitleBarNormalButton, Qx::DockPanelUndockIcon);
    m_undockButton->setSizePolicy(sp);
    m_layout->addWidget(m_undockButton, 0);
    q->connect(m_undockButton, SIGNAL(clicked()), SLOT(onUndockButtonClicked()));

    // AutoHide Button
    const auto autoHideEnabled = testAutoHideConfigFlag(DockManager::AutoHideFeatureEnabled);
    m_autoHideButton =
        new TitleBarButton(testAutoHideConfigFlag(DockManager::DockAreaHasAutoHideButton) && autoHideEnabled, true,
                           Qx::TitleBarButtonAutoHide);
    m_autoHideButton->setObjectName("dockPanelAutoHideButton");
    m_autoHideButton->setAutoRaise(true);
    internal::setToolTip(m_autoHideButton, q->titleBarButtonToolTip(Qx::TitleBarButtonAutoHide));
    internal::setButtonIcon(m_autoHideButton, QStyle::SP_DialogOkButton, Qx::DockAutoHideIcon);
    m_autoHideButton->setSizePolicy(sp);
    m_autoHideButton->setCheckable(testAutoHideConfigFlag(DockManager::AutoHideButtonCheckable));
    m_autoHideButton->setChecked(false);
    m_layout->addWidget(m_autoHideButton, 0);
    q->connect(m_autoHideButton, SIGNAL(clicked()), SLOT(onAutoHideButtonClicked()));

    // Minimize button
    m_minimizeButton = new TitleBarButton(testAutoHideConfigFlag(DockManager::AutoHideHasMinimizeButton), false,
                                          Qx::TitleBarButtonMinimize);
    m_minimizeButton->setObjectName("dockPanelMinimizeButton");
    m_minimizeButton->setAutoRaise(true);
    m_minimizeButton->setVisible(false);
    internal::setButtonIcon(m_minimizeButton, QStyle::SP_TitleBarMinButton, Qx::DockPanelMinimizeIcon);
    internal::setToolTip(m_minimizeButton, QObject::tr("Minimize"));
    m_minimizeButton->setSizePolicy(sp);
    m_layout->addWidget(m_minimizeButton, 0);
    q->connect(m_minimizeButton, SIGNAL(clicked()), SLOT(minimizeAutoHideContainer()));

    // Close button
    m_closeButton =
        new TitleBarButton(testConfigFlag(DockManager::DockAreaHasCloseButton), true, Qx::TitleBarButtonClose);
    m_closeButton->setObjectName("dockPanelCloseButton");
    m_closeButton->setAutoRaise(true);
    internal::setButtonIcon(m_closeButton, QStyle::SP_TitleBarCloseButton, Qx::DockPanelCloseIcon);
    internal::setToolTip(m_closeButton, q->titleBarButtonToolTip(Qx::TitleBarButtonClose));
    m_closeButton->setSizePolicy(sp);
    m_closeButton->setIconSize(QSize(16, 16));
    m_layout->addWidget(m_closeButton, 0);
    q->connect(m_closeButton, SIGNAL(clicked()), SLOT(onCloseButtonClicked()));
}

void DockTitleBarPrivate::createAutoHideTitleLabel()
{
    Q_Q(DockTitleBar);
    m_autoHideTitleLabel = new DockLabel("");
    m_autoHideTitleLabel->setObjectName("autoHideTitleLabel");
    // At position 0 is the tab bar - insert behind tab bar
    m_layout->insertWidget(1, m_autoHideTitleLabel);
    m_autoHideTitleLabel->setVisible(false);   // Default hidden
    m_layout->insertWidget(2, new SpacerWidget(q));
}

DockWindow *DockTitleBarPrivate::dockWindow() const
{
    return m_panel->dockWindow();
}

bool DockTitleBarPrivate::isDraggingState(Qx::DockDragState dragState) const
{
    return m_dragState == dragState;
}

void DockTitleBarPrivate::startFloating(const QPoint &offset)
{
    m_floatingWidget = makeAreaFloating(offset, Qx::DockDraggingFloatingWidget);
    qApp->postEvent(m_panel, new QEvent((QEvent::Type)internal::DockedWidgetDragStartEvent));
}

DockFloatingWidget *DockTitleBarPrivate::makeAreaFloating(const QPoint &offset, Qx::DockDragState dragState)
{
    QSize sz = m_panel->size();
    m_dragState = dragState;
    bool needCreateFloatingContainer = (dragState != Qx::DockDraggingFloatingWidget);
    DockFloatingContainer *floatingContainer = nullptr;
    DockFloatingWidget *floatingWidget;
    if (needCreateFloatingContainer) {
        floatingWidget = floatingContainer = new DockFloatingContainer(m_panel);
    } else {
        auto w = new DockFloatingPreview(m_panel);
        QObject::connect(w, &DockFloatingPreview::draggingCanceled, [=]() {
            this->m_dragState = Qx::DockDraggingInactive;
        });
        floatingWidget = w;
    }
    floatingWidget->startFloating(offset, sz, dragState, nullptr);
    if (floatingContainer) {
        auto topLevelWidget = floatingContainer->topLevelDockWidget();
        if (topLevelWidget) {
            topLevelWidget->emitTopLevelChanged(true);
        }
    }
    return floatingWidget;
}

bool DockTitleBarPrivate::testConfigFlag(DockManager::ConfigFlag flag)
{
    return DockManager::testConfigFlag(flag);
}

bool DockTitleBarPrivate::testAutoHideConfigFlag(DockManager::AutoHideFlag flag)
{
    return DockManager::testAutoHideConfigFlag(flag);
}

QAction *DockTitleBarPrivate::createAutoHideToAction(const QString &title, Qx::DockSideBarArea area, QMenu *menu)
{
    Q_Q(DockTitleBar);
    auto action = menu->addAction(title);
    action->setProperty(internal::LocationProperty, area);
    QObject::connect(action, &QAction::triggered, q, &DockTitleBar::onAutoHideToActionClicked);
    return action;
}

DockTitleBar::DockTitleBar(DockPanel *parent)
    : QWidget(parent)
{
    QX_INIT_PRIVATE(DockTitleBar);
    Q_D(DockTitleBar);
    d->m_panel = parent;
    d->init();

    setObjectName("dockTitleBar");
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setFocusPolicy(Qt::NoFocus);
}

DockTitleBar::~DockTitleBar()
{
    QX_FINI_PRIVATE();
}

DockTabBar *DockTitleBar::tabBar() const
{
    Q_D(const DockTitleBar);
    return d->m_tabBar;
}

DockPanel *DockTitleBar::dockPanel() const
{
    Q_D(const DockTitleBar);
    return d->m_panel;
}

QAbstractButton *DockTitleBar::button(Qx::DockTitleBarButton which) const
{
    Q_D(const DockTitleBar);

    switch (which) {
    case Qx::TitleBarButtonTabsMenu:
        return d->m_tabsMenuButton;
    case Qx::TitleBarButtonUndock:
        return d->m_undockButton;
    case Qx::TitleBarButtonClose:
        return d->m_closeButton;
    case Qx::TitleBarButtonAutoHide:
        return d->m_autoHideButton;
    case Qx::TitleBarButtonMinimize:
        return d->m_minimizeButton;
    default:
        return nullptr;
    }
}

DockLabel *DockTitleBar::autoHideTitleLabel() const
{
    Q_D(const DockTitleBar);
    return d->m_autoHideTitleLabel;
}

void DockTitleBar::showAutoHideControls(bool show)
{
    Q_D(DockTitleBar);
    d->m_tabBar->setVisible(!show);   // Auto hide toolbar never has tabs
    d->m_minimizeButton->setVisible(show);
    d->m_autoHideTitleLabel->setVisible(show);
}

void DockTitleBar::updateDockWidgetActionsButtons()
{
    Q_D(DockTitleBar);
    auto tab = d->m_tabBar->currentTab();
    if (!tab) {
        return;
    }
    DockWidget *w = tab->dockWidget();
    // clear old buttons
    if (!d->m_dockWidgetActionsButtons.isEmpty()) {
        for (auto b : d->m_dockWidgetActionsButtons) {
            d->m_layout->removeWidget(b);
            delete b;
        }
        d->m_dockWidgetActionsButtons.clear();
    }
    auto actions = w->titleBarActions();
    if (actions.isEmpty()) {
        return;
    }
    int insertIndex = indexOf(d->m_tabsMenuButton);
    for (auto a : actions) {
        auto btn = new TitleBarButton(true, false, Qx::TitleBarButtonTabsMenu, this);
        btn->setDefaultAction(a);
        btn->setAutoRaise(true);
        btn->setPopupMode(QToolButton::InstantPopup);
        btn->setObjectName(a->objectName());
        d->m_layout->insertWidget(insertIndex++, btn, 0);
        d->m_dockWidgetActionsButtons.append(btn);
    }
}

int DockTitleBar::indexOf(QWidget *widget) const
{
    Q_D(const DockTitleBar);
    return d->m_layout->indexOf(widget);
}

QString DockTitleBar::titleBarButtonToolTip(Qx::DockTitleBarButton id) const
{
    return QString();
}

void DockTitleBar::markTabsMenuOutdated()
{
    Q_D(DockTitleBar);
    if (DockManager::testConfigFlag(DockManager::DockAreaDynamicTabsMenuButtonVisibility)) {
        bool tabsMenuButtonVisible = false;
        if (DockManager::testConfigFlag(DockManager::DisableTabTextEliding)) {
            tabsMenuButtonVisible = d->m_tabBar->areTabsOverflowing();
        } else {
            bool hasElidedTabTitle = false;
            for (int i = 0; i < d->m_tabBar->count(); ++i) {
                if (!d->m_tabBar->isTabOpen(i)) {
                    continue;
                }
                DockTab *tab = d->m_tabBar->tab(i);
                if (tab->isTitleElided()) {
                    hasElidedTabTitle = true;
                    break;
                }
            }
            tabsMenuButtonVisible = (hasElidedTabTitle && (d->m_tabBar->count() > 1));
        }
        QMetaObject::invokeMethod(d->m_tabsMenuButton, "setVisible", Qt::QueuedConnection,
                                  Q_ARG(bool, tabsMenuButtonVisible));
    }
    d->m_menuOutdated = true;
}

void DockTitleBar::onTabsMenuAboutToShow()
{
    Q_D(DockTitleBar);
    if (!d->m_menuOutdated) {
        return;
    }
    QMenu *menu = d->m_tabsMenuButton->menu();
    menu->clear();
    for (int i = 0; i < d->m_tabBar->count(); ++i) {
        if (!d->m_tabBar->isTabOpen(i)) {
            continue;
        }
        auto tab = d->m_tabBar->tab(i);
        QAction *action = menu->addAction(tab->icon(), tab->text());
        internal::setToolTip(action, tab->toolTip());
        action->setData(i);
    }
    d->m_menuOutdated = false;
}

void DockTitleBar::onTabsMenuActionTriggered(QAction *a)
{
    Q_D(DockTitleBar);
    int index = a->data().toInt();
    d->m_tabBar->setCurrentIndex(index);
    Q_EMIT tabBarClicked(index);
}

void DockTitleBar::onCurrentTabChanged(int index)
{
    if (index < 0) {
        return;
    }
    Q_D(DockTitleBar);
    if (d->testConfigFlag(DockManager::DockAreaCloseButtonClosesTab)) {
        DockWidget *w = d->m_tabBar->tab(index)->dockWidget();
        d->m_closeButton->setEnabled(w->features().testFlag(DockWidget::DockWidgetClosable));
    }
    updateDockWidgetActionsButtons();
}

void DockTitleBar::onCloseButtonClicked()
{
    Q_D(DockTitleBar);
    if (DockManager::testAutoHideConfigFlag(DockManager::AutoHideCloseButtonCollapsesDock) &&
        d->m_panel->autoHideContainer()) {
        d->m_panel->autoHideContainer()->collapseView(true);
    } else if (d->testConfigFlag(DockManager::DockAreaCloseButtonClosesTab)) {
        d->m_tabBar->closeTab(d->m_tabBar->currentIndex());
    } else {
        d->m_panel->closeArea();
    }
}

void DockTitleBar::onUndockButtonClicked()
{
    Q_D(DockTitleBar);
    if (d->m_panel->features().testFlag(DockWidget::DockWidgetFloatable)) {
        d->makeAreaFloating(mapFromGlobal(QCursor::pos()), Qx::DockDraggingInactive);
    }
}

void DockTitleBar::onAutoHideButtonClicked()
{
    Q_D(DockTitleBar);
    if (DockManager::testAutoHideConfigFlag(DockManager::AutoHideButtonTogglesArea) ||
        qApp->keyboardModifiers().testFlag(Qt::ControlModifier)) {
        d->m_panel->toggleAutoHide();
    } else {
        d->m_panel->currentDockWidget()->toggleAutoHide();
    }
}

void DockTitleBar::minimizeAutoHideContainer()
{
    Q_D(DockTitleBar);
    auto autoHideContainer = d->m_panel->autoHideContainer();
    if (autoHideContainer) {
        autoHideContainer->collapseView(true);
    }
}

void DockTitleBar::onAutoHideDockAreaActionClicked()
{
    Q_D(DockTitleBar);
    d->m_panel->toggleAutoHide();
}

void DockTitleBar::onAutoHideToActionClicked()
{
    Q_D(DockTitleBar);
    int location = sender()->property(internal::LocationProperty).toInt();
    d->m_panel->toggleAutoHide((Qx::DockSideBarArea)location);
}

void DockTitleBar::onAutoHideCloseActionTriggered()
{
    Q_D(DockTitleBar);
    d->m_panel->closeArea();
}

void DockTitleBar::mousePressEvent(QMouseEvent *e)
{
    Q_D(DockTitleBar);
    if (e->button() == Qt::LeftButton) {
        e->accept();
        d->m_dragStartMousePos = e->pos();
        d->m_dragState = Qx::DockDraggingMousePressed;
        if (DockManager::testConfigFlag(DockManager::FocusHighlighting)) {
            d->dockWindow()->dockFocusController()->setDockWidgetTabFocused(d->m_tabBar->currentTab());
        }
        return;
    }
    Super::mousePressEvent(e);
}

void DockTitleBar::mouseReleaseEvent(QMouseEvent *e)
{
    Q_D(DockTitleBar);
    if (e->button() == Qt::LeftButton) {
        e->accept();
        auto currentDragState = d->m_dragState;
        d->m_dragStartMousePos = QPoint();
        d->m_dragState = Qx::DockDraggingInactive;
        if (Qx::DockDraggingFloatingWidget == currentDragState) {
            d->m_floatingWidget->finishDragging();
        }

        return;
    }
    Super::mouseReleaseEvent(e);
}

void DockTitleBar::mouseMoveEvent(QMouseEvent *e)
{
    Q_D(DockTitleBar);
    Super::mouseMoveEvent(e);
    if (!(e->buttons() & Qt::LeftButton) || d->isDraggingState(Qx::DockDraggingInactive)) {
        d->m_dragState = Qx::DockDraggingInactive;
        return;
    }

    // move floating window
    if (d->isDraggingState(Qx::DockDraggingFloatingWidget)) {
        d->m_floatingWidget->moveFloating();
        return;
    }

    // If this is the last dock area in a floating dock container it does not make
    // sense to move it to a new floating widget and leave this one empty
    if (d->m_panel->dockContainer()->isFloating() && d->m_panel->dockContainer()->visibleDockPanelCount() == 1 &&
        !d->m_panel->isAutoHide()) {
        return;
    }

    // If one single dock widget in this area is not floatable then the whole
    // area is not floatable
    // We can create the floating drag preview if the dock widget is movable
    auto features = d->m_panel->features();
    if (!features.testFlag(DockWidget::DockWidgetFloatable) && !(features.testFlag(DockWidget::DockWidgetMovable))) {
        return;
    }

    int dragDistance = (d->m_dragStartMousePos - e->pos()).manhattanLength();
    if (dragDistance >= DockManager::startDragDistance()) {
        d->startFloating(d->m_dragStartMousePos);
        auto overlay = d->m_panel->dockWindow()->containerOverlay();
        overlay->setAllowedAreas(Qx::OuterDockAreas);
    }

    return;
}

void DockTitleBar::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_D(DockTitleBar);
    // If this is the last dock area in a dock container it does not make
    // sense to move it to a new floating widget and leave this one empty
    if (d->m_panel->dockContainer()->isFloating() && d->m_panel->dockContainer()->dockPanelCount() == 1) {
        return;
    }

    if (!d->m_panel->features().testFlag(DockWidget::DockWidgetFloatable)) {
        return;
    }

    d->makeAreaFloating(e->pos(), Qx::DockDraggingInactive);
}

void DockTitleBar::contextMenuEvent(QContextMenuEvent *e)
{
    Q_D(DockTitleBar);
    e->accept();
    if (d->isDraggingState(Qx::DockDraggingFloatingWidget)) {
        return;
    }

    const bool isAutoHide = d->m_panel->isAutoHide();
    const bool isTopLevelArea = d->m_panel->isTopLevelArea();
    QAction *action;
    QMenu menu(this);
    if (!isTopLevelArea) {
        action = menu.addAction(isAutoHide ? tr("Detach") : tr("Detach Group"), this, SLOT(onUndockButtonClicked()));
        action->setEnabled(d->m_panel->features().testFlag(DockWidget::DockWidgetFloatable));
        if (DockManager::testAutoHideConfigFlag(DockManager::AutoHideFeatureEnabled)) {
            action = menu.addAction(isAutoHide ? tr("Unpin (Dock)") : tr("Pin Group"), this,
                                    SLOT(onAutoHideDockAreaActionClicked()));
            auto areaIsPinnable = d->m_panel->features().testFlag(DockWidget::DockWidgetPinnable);
            action->setEnabled(areaIsPinnable);

            if (!isAutoHide) {
                auto pinMenu = menu.addMenu(tr("Pin Group To..."));
                pinMenu->setEnabled(areaIsPinnable);
                d->createAutoHideToAction(tr("Top"), Qx::DockSideBarTop, pinMenu);
                d->createAutoHideToAction(tr("Left"), Qx::DockSideBarLeft, pinMenu);
                d->createAutoHideToAction(tr("Right"), Qx::DockSideBarRight, pinMenu);
                d->createAutoHideToAction(tr("Bottom"), Qx::DockSideBarBottom, pinMenu);
            }
        }
        menu.addSeparator();
    }

    if (isAutoHide) {
        action = menu.addAction(tr("Minimize"), this, SLOT(minimizeAutoHideContainer()));
        action = menu.addAction(tr("Close"), this, SLOT(onAutoHideCloseActionTriggered()));
    } else {
        action = menu.addAction(isAutoHide ? tr("Close") : tr("Close Group"), this, SLOT(onCloseButtonClicked()));
    }

    action->setEnabled(d->m_panel->features().testFlag(DockWidget::DockWidgetClosable));
    if (!isAutoHide && !isTopLevelArea) {
        action = menu.addAction(tr("Close Other Groups"), d->m_panel, SLOT(closeOtherAreas()));
    }
    menu.exec(e->globalPos());
}

void DockTitleBar::resizeEvent(QResizeEvent *e)
{
    Super::resizeEvent(e);
    if (DockManager::testConfigFlag(DockManager::DockAreaDynamicTabsMenuButtonVisibility) &&
        DockManager::testConfigFlag(DockManager::DisableTabTextEliding)) {
        markTabsMenuOutdated();
    }
}

QX_DOCK_END_NAMESPACE
