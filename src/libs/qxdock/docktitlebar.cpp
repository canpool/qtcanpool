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

#include <QBoxLayout>
#include <QMenu>

QX_DOCK_BEGIN_NAMESPACE


TitleBarButton::TitleBarButton(bool showInTitleBar, bool hideWhenDisabled, Qx::DockTitleBarButton id, QWidget *parent)
    : QToolButton(parent)
    , m_id(id)
    , m_showInTitleBar(showInTitleBar)
    , m_hideWhenDisabled(hideWhenDisabled)
{
    setFocusPolicy(Qt::NoFocus);
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

    DockFloatingWidget *makeAreaFloating(const QPoint &offset, Qx::DockDragState dragState);

    static bool testConfigFlag(DockManager::ConfigFlag flag);
public:
    DockPanel *m_panel = nullptr;
    QBoxLayout *m_layout = nullptr;
    DockTabBar *m_tabBar = nullptr;

    QPointer<TitleBarButton> m_tabsMenuButton;
    QPointer<TitleBarButton> m_undockButton;
    QPointer<TitleBarButton> m_closeButton;
    QPointer<TitleBarButton> m_minimizeButton;

    QList<TitleBarButton *> m_dockWidgetActionsButtons;

    Qx::DockDragState m_dragState = Qx::DockDraggingInactive;
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
}

void DockTitleBarPrivate::createTabBar()
{
    Q_Q(DockTitleBar);
    m_tabBar = new DockTabBar(m_panel);
    // The horizontal direction uses Expanding instead of Maximum to maximize expansion
    // I don't know why QSizePolicy::Maximum doesn't work good
    m_tabBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_layout->addWidget(m_tabBar);
    q->connect(m_tabBar, SIGNAL(currentChanged(int)), SLOT(onCurrentTabChanged(int)));
    q->connect(m_tabBar, SIGNAL(tabBarClicked(int)), SIGNAL(tabBarClicked(int)));
}

void DockTitleBarPrivate::createButtons()
{
    Q_Q(DockTitleBar);

    QSizePolicy sp(QSizePolicy::Fixed, QSizePolicy::Expanding);

    // Tabs menu button
    m_tabsMenuButton = new TitleBarButton(testConfigFlag(DockManager::DockAreaHasTabsMenuButton),
                                          false, Qx::TitleBarButtonTabsMenu);
    m_tabsMenuButton->setObjectName("tabsMenuButton");
    m_tabsMenuButton->setAutoRaise(true);
    m_tabsMenuButton->setPopupMode(QToolButton::InstantPopup);
    internal::setButtonIcon(m_tabsMenuButton, QStyle::SP_TitleBarUnshadeButton, Qx::DockPanelMenuIcon);
    QMenu *tabsMenu = new QMenu(m_tabsMenuButton);
#ifndef QT_NO_TOOLTIP
    tabsMenu->setToolTipsVisible(true);
#endif
    q->connect(tabsMenu, SIGNAL(aboutToShow()), SLOT(onTabsMenuAboutToShow()));
    q->connect(tabsMenu, SIGNAL(triggered(QAction*)), SLOT(onTabsMenuActionTriggered(QAction*)));
    m_tabsMenuButton->setMenu(tabsMenu);
    internal::setToolTip(m_tabsMenuButton, QObject::tr("List All Tabs"));
    m_tabsMenuButton->setSizePolicy(sp);
    m_layout->addWidget(m_tabsMenuButton, 0);

    // Undock button
    m_undockButton = new TitleBarButton(testConfigFlag(DockManager::DockAreaHasUndockButton),
                                        true, Qx::TitleBarButtonUndock);
    m_undockButton->setObjectName("detachGroupButton");
    m_undockButton->setAutoRaise(true);
    internal::setToolTip(m_undockButton, QObject::tr("Detach Group"));
    internal::setButtonIcon(m_undockButton, QStyle::SP_TitleBarNormalButton, Qx::DockPanelUndockIcon);
    m_undockButton->setSizePolicy(sp);
    m_layout->addWidget(m_undockButton, 0);
    q->connect(m_undockButton, SIGNAL(clicked()), SLOT(onUndockButtonClicked()));

    // Minimize button
    m_minimizeButton = new TitleBarButton(true, false, Qx::TitleBarButtonMinimize);
    m_minimizeButton->setObjectName("dockPanelMinimizeButton");
    m_minimizeButton->setAutoRaise(true);
    m_minimizeButton->setVisible(false);
    internal::setButtonIcon(m_minimizeButton, QStyle::SP_TitleBarMinButton, Qx::DockPanelMinimizeIcon);
    internal::setToolTip(m_minimizeButton, QObject::tr("Minimize"));
    m_minimizeButton->setSizePolicy(sp);
    m_layout->addWidget(m_minimizeButton, 0);

    // Close button
    m_closeButton = new TitleBarButton(testConfigFlag(DockManager::DockAreaHasCloseButton),
                                       true, Qx::TitleBarButtonClose);
    m_closeButton->setObjectName("dockPanelCloseButton");
    m_closeButton->setAutoRaise(true);
    internal::setButtonIcon(m_closeButton, QStyle::SP_TitleBarCloseButton, Qx::DockPanelCloseIcon);
    internal::setToolTip(m_closeButton, q->titleBarButtonToolTip(Qx::TitleBarButtonClose));
    m_closeButton->setSizePolicy(sp);
    m_closeButton->setIconSize(QSize(16, 16));
    m_layout->addWidget(m_closeButton, 0);
    q->connect(m_closeButton, SIGNAL(clicked()), SLOT(onCloseButtonClicked()));
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
        auto w = new DockFloatingPreview();
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
    QX_FINI_PRIVATE()
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

    switch (which)
    {
    case Qx::TitleBarButtonTabsMenu: return d->m_tabsMenuButton;
    case Qx::TitleBarButtonUndock: return d->m_undockButton;
    case Qx::TitleBarButtonClose: return d->m_closeButton;
    case Qx::TitleBarButtonMinimize: return d->m_minimizeButton;
    default:
        return nullptr;
    }
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
    for (auto a: actions) {
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

void DockTitleBar::onTabsMenuAboutToShow()
{
    Q_D(DockTitleBar);
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
    if (d->testConfigFlag(DockManager::DockAreaCloseButtonClosesTab)) {
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


QX_DOCK_END_NAMESPACE
