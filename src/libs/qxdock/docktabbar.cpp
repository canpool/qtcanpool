/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "docktabbar.h"
#include "dockpanel.h"
#include "docktab.h"

#include <QBoxLayout>

QX_DOCK_BEGIN_NAMESPACE

class DockTabBarPrivate
{
public:
    QX_DECLARE_PUBLIC(DockTabBar)
public:
    DockTabBarPrivate();
    void init();
public:
    DockPanel *m_panel = nullptr;
    QWidget *m_tabsContainerWidget;
    QBoxLayout *m_tabsLayout;
};

DockTabBarPrivate::DockTabBarPrivate()
{

}

void DockTabBarPrivate::init()
{
    Q_Q(DockTabBar);

    m_tabsContainerWidget = new QWidget();
    m_tabsContainerWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_tabsContainerWidget->setObjectName("tabsContainerWidget");
    m_tabsLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    m_tabsLayout->setContentsMargins(0, 0, 0, 0);
    m_tabsLayout->setSpacing(0);
    m_tabsLayout->addStretch(1);
    m_tabsContainerWidget->setLayout(m_tabsLayout);
    q->setWidget(m_tabsContainerWidget);
}

DockTabBar::DockTabBar(DockPanel *parent)
    : QScrollArea(parent)
{
    QX_INIT_PRIVATE(DockTabBar)
    Q_D(DockTabBar);
    d->m_panel = parent;
    d->init();

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setFrameStyle(QFrame::NoFrame);
    setWidgetResizable(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFocusPolicy(Qt::NoFocus);
}

DockTabBar::~DockTabBar()
{
    QX_FINI_PRIVATE()
}

QX_DOCK_END_NAMESPACE
