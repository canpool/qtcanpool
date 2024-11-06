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
    void updateTabs();
public:
    DockPanel *m_panel = nullptr;
    QWidget *m_tabsContainerWidget;
    QBoxLayout *m_tabsLayout;
    int m_currentIndex = -1;
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

void DockTabBarPrivate::updateTabs()
{
    Q_Q(DockTabBar);
    for (int i = 0; i < q->count(); ++i) {
        auto tab = q->tab(i);
        if (!tab) {
            continue;
        }
        if (i == m_currentIndex) {
            tab->show();
            tab->setActive(true);
        } else {
            tab->setActive(false);
        }
    }
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

void DockTabBar::insertTab(int index, DockTab *tab)
{
    Q_D(DockTabBar);
    d->m_tabsLayout->insertWidget(index, tab);
    if (index <= d->m_currentIndex) {
        setCurrentIndex(d->m_currentIndex + 1);
    } else if (d->m_currentIndex == -1) {
        setCurrentIndex(index);
    }
    updateGeometry();
}

int DockTabBar::count() const
{
    Q_D(const DockTabBar);
    // The tab bar contains a stretch item as last item
    return d->m_tabsLayout->count() - 1;
}

DockTab *DockTabBar::tab(int index) const
{
    if (index < 0 || index >= count()) {
        return nullptr;
    }
    Q_D(const DockTabBar);
    return qobject_cast<DockTab *>(d->m_tabsLayout->itemAt(index)->widget());
}

QSize DockTabBar::minimumSizeHint() const
{
    QSize Size = sizeHint();
    Size.setWidth(10);
    return Size;
}

QSize DockTabBar::sizeHint() const
{
    Q_D(const DockTabBar);
    // Limit the size of the entire QScrollArea based on the size of the widgets in the container,
    // otherwise the size of the QScrollArea will be large.
    // Notes: setting SizePolicy alone will not make the size smaller.
    return d->m_tabsContainerWidget->sizeHint();
}

void DockTabBar::setCurrentIndex(int index)
{
    Q_D(DockTabBar);
    if (index == d->m_currentIndex) {
        return;
    }
    if (index < -1 || index > (count() -1)) {
        return;
    }
    d->m_currentIndex = index;
    d->updateTabs();
    updateGeometry();
}

QX_DOCK_END_NAMESPACE
