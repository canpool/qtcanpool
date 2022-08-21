/**
 * Copyright (C) 2018-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#include "fancytabwidget.h"
#include "fancytabbar.h"

#include <QStackedWidget>
#include <QBoxLayout>
#include <QIcon>

QCANPOOL_BEGIN_NAMESPACE

class FancyTabWidgetPrivate
{
public:
    FancyTabWidgetPrivate();

    void init();
    void updateTabBarPosition();

    FancyTabBar *m_tabBar;
    QStackedWidget *m_stack;
    QBoxLayout *m_layout;

    FancyTabWidget *q;
    FancyTabWidget::TabPosition m_pos;
};

FancyTabWidgetPrivate::FancyTabWidgetPrivate()
    : q(nullptr)
    , m_pos(FancyTabWidget::North)
{
}

void FancyTabWidgetPrivate::init()
{
    m_tabBar = new FancyTabBar(q);
    m_tabBar->setObjectName(QLatin1String("qtc_tabwidget_tabbar"));

    m_stack = new QStackedWidget(q);
    m_stack->setObjectName(QLatin1String("qtc_tabwidget_stackedwidget"));
    m_stack->setLineWidth(0);

    m_layout = new QBoxLayout(QBoxLayout::TopToBottom);
    m_layout->setMargin(0);
    m_layout->setSpacing(1);
    m_layout->addWidget(m_tabBar);
    m_layout->addWidget(m_stack);
    q->setLayout(m_layout);

    QObject::connect(m_tabBar, SIGNAL(currentChanged(int)), m_stack, SLOT(setCurrentIndex(int)));
}

void FancyTabWidgetPrivate::updateTabBarPosition()
{
    switch (m_pos) {
        case FancyTabWidget::North: {
            m_layout->setDirection(QBoxLayout::TopToBottom);
            m_tabBar->setOrientation(Qt::Horizontal);
            break;
        }
        case FancyTabWidget::South: {
            m_layout->setDirection(QBoxLayout::BottomToTop);
            m_tabBar->setOrientation(Qt::Horizontal);
            break;
        }
        case FancyTabWidget::West: {
            m_layout->setDirection(QBoxLayout::LeftToRight);
            m_tabBar->setOrientation(Qt::Vertical);
            break;
        }
        case FancyTabWidget::East: {
            m_layout->setDirection(QBoxLayout::RightToLeft);
            m_tabBar->setOrientation(Qt::Vertical);
            break;
        }
    }
}

FancyTabWidget::FancyTabWidget(QWidget *parent)
    : QWidget(parent), d(new FancyTabWidgetPrivate())
{
    setAttribute(Qt::WA_StyledBackground, true);
    setObjectName(QLatin1String("qtc_tabwidget"));

    d->q = this;
    d->init();
}

FancyTabWidget::~FancyTabWidget()
{
    delete d;
}

int FancyTabWidget::addTab(QWidget *widget, const QString &label)
{
    return insertTab(-1, widget, label);
}

int FancyTabWidget::addTab(QWidget *widget, const QIcon &icon, const QString &label)
{
    return insertTab(-1, widget, icon, label);
}

int FancyTabWidget::insertTab(int index, QWidget *widget, const QString &label)
{
    return insertTab(index, widget, QIcon(), label);
}

int FancyTabWidget::insertTab(int index, QWidget *widget, const QIcon &icon, const QString &label)
{
    if(!widget)
        return -1;
    index = d->m_stack->insertWidget(index, widget);
    d->m_tabBar->insertTab(index, icon, label);

    return index;
}

void FancyTabWidget::removeTab(int index)
{
    if (QWidget *w = d->m_stack->widget(index)) {
        d->m_stack->removeWidget(w);
        d->m_tabBar->removeTab(index);
    }
}

int FancyTabWidget::currentIndex() const
{
    return d->m_stack->currentIndex();
}

QWidget *FancyTabWidget::currentWidget() const
{
    return d->m_stack->currentWidget();
}

QWidget *FancyTabWidget::widget(int index) const
{
    return d->m_stack->widget(index);
}

int FancyTabWidget::indexOf(QWidget *widget) const
{
    return d->m_stack->indexOf(widget);
}

int FancyTabWidget::count() const
{
    return d->m_tabBar->count();
}

void FancyTabWidget::setTabEnabled(QWidget *widget, bool enable)
{
    int index = d->m_stack->indexOf(widget);
    d->m_tabBar->setTabEnabled(index, enable);
}

void FancyTabWidget::setTabVisible(QWidget *widget, bool visible)
{
    int index = d->m_stack->indexOf(widget);
    d->m_tabBar->setTabVisible(index, visible);
}

FancyTabBar *FancyTabWidget::tabBar() const
{
    return d->m_tabBar;
}

FancyTabWidget::TabPosition FancyTabWidget::tabPosition() const
{
    return d->m_pos;
}

void FancyTabWidget::setTabPosition(FancyTabWidget::TabPosition pos)
{
    if (d->m_pos == pos)
        return;
    d->m_pos = pos;
    d->updateTabBarPosition();
}

QCANPOOL_END_NAMESPACE
