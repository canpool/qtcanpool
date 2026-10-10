/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxtabwidget.h"
#include "qxtabbar.h"

#include <QStackedWidget>
#include <QToolButton>
#include <QBoxLayout>
#include <QIcon>

QX_APP_BEGIN_NAMESPACE

class QxTabWidgetPrivate
{
    QX_DECLARE_PUBLIC(QxTabWidget)
public:
    QxTabWidgetPrivate();

    void init();
    void updateTabBarPosition();
public:
    QxTabBar *m_tabBar;
    QStackedWidget *m_stack;
    QBoxLayout *m_layout;
    QBoxLayout *m_tabLayout;
    QxTabWidget::TabPosition m_pos;
};

QxTabWidgetPrivate::QxTabWidgetPrivate()
    : m_pos(QxTabWidget::North)
{
}

void QxTabWidgetPrivate::init()
{
    Q_Q(QxTabWidget);
    m_tabBar = new QxTabBar(q);
    m_tabBar->setTogglable(false);
    m_tabBar->setObjectName(QLatin1String("qtc_qxtabwidget_tabbar"));

    m_stack = new QStackedWidget(q);
    m_stack->setObjectName(QLatin1String("qtc_qxtabwidget_stackedwidget"));
    m_stack->setLineWidth(0);

    m_tabLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    m_tabLayout->setContentsMargins(0, 0, 0, 0);
    m_tabLayout->setSpacing(0);
    m_tabLayout->addWidget(m_tabBar);
    m_tabLayout->addStretch();

    m_layout = new QBoxLayout(QBoxLayout::TopToBottom);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    m_layout->addLayout(m_tabLayout);
    m_layout->addWidget(m_stack);
    q->setLayout(m_layout);

    QObject::connect(m_tabBar, SIGNAL(currentChanged(int)), m_stack, SLOT(setCurrentIndex(int)));
    QObject::connect(m_stack, SIGNAL(currentChanged(int)), q, SIGNAL(currentChanged(int)));

    updateTabBarPosition();
}

void QxTabWidgetPrivate::updateTabBarPosition()
{
    switch (m_pos) {
    case QxTabWidget::North: {
        m_layout->setDirection(QBoxLayout::TopToBottom);
        m_tabLayout->setDirection(QBoxLayout::LeftToRight);
        m_tabBar->setOrientation(Qt::Horizontal);
        break;
    }
    case QxTabWidget::South: {
        m_layout->setDirection(QBoxLayout::BottomToTop);
        m_tabLayout->setDirection(QBoxLayout::LeftToRight);
        m_tabBar->setOrientation(Qt::Horizontal);
        break;
    }
    case QxTabWidget::West: {
        m_layout->setDirection(QBoxLayout::LeftToRight);
        m_tabLayout->setDirection(QBoxLayout::TopToBottom);
        m_tabBar->setOrientation(Qt::Vertical);
        break;
    }
    case QxTabWidget::East: {
        m_layout->setDirection(QBoxLayout::RightToLeft);
        m_tabLayout->setDirection(QBoxLayout::TopToBottom);
        m_tabBar->setOrientation(Qt::Vertical);
        break;
    }
    }
}

QxTabWidget::QxTabWidget(QWidget *parent)
    : QWidget(parent)
{
    QX_INIT_PRIVATE(QxTabWidget)
    Q_D(QxTabWidget);
    d->init();

    setAttribute(Qt::WA_StyledBackground, true);
}

QxTabWidget::~QxTabWidget()
{
    QX_FINI_PRIVATE()
}

int QxTabWidget::addTab(QWidget *widget, const QString &label)
{
    return insertTab(-1, widget, label);
}

int QxTabWidget::addTab(QWidget *widget, const QIcon &icon, const QString &label)
{
    return insertTab(-1, widget, icon, label);
}

int QxTabWidget::insertTab(int index, QWidget *widget, const QString &label)
{
    return insertTab(index, widget, QIcon(), label);
}

int QxTabWidget::insertTab(int index, QWidget *widget, const QIcon &icon, const QString &label)
{
    Q_D(QxTabWidget);
    if (!widget) {
        return -1;
    }
    index = d->m_stack->insertWidget(index, widget);
    d->m_tabBar->insertTab(index, icon, label);

    return index;
}

void QxTabWidget::removeTab(int index)
{
    Q_D(QxTabWidget);
    if (QWidget *w = d->m_stack->widget(index)) {
        d->m_stack->removeWidget(w);
        d->m_tabBar->removeTab(index);
    }
}

int QxTabWidget::currentIndex() const
{
    Q_D(const QxTabWidget);
    return d->m_stack->currentIndex();
}

QWidget *QxTabWidget::currentWidget() const
{
    Q_D(const QxTabWidget);
    return d->m_stack->currentWidget();
}

QWidget *QxTabWidget::widget(int index) const
{
    Q_D(const QxTabWidget);
    return d->m_stack->widget(index);
}

int QxTabWidget::indexOf(QWidget *widget) const
{
    Q_D(const QxTabWidget);
    return d->m_stack->indexOf(widget);
}

int QxTabWidget::count() const
{
    Q_D(const QxTabWidget);
    return d->m_tabBar->count();
}

void QxTabWidget::setTabEnabled(int index, bool enable)
{
    Q_D(QxTabWidget);
    d->m_tabBar->setTabEnabled(index, enable);
}

void QxTabWidget::setTabVisible(int index, bool visible)
{
    Q_D(QxTabWidget);
    d->m_tabBar->setTabVisible(index, visible);
}

QxTabBar *QxTabWidget::tabBar() const
{
    Q_D(const QxTabWidget);
    return d->m_tabBar;
}

QxTabWidget::TabPosition QxTabWidget::tabPosition() const
{
    Q_D(const QxTabWidget);
    return d->m_pos;
}

void QxTabWidget::setTabPosition(QxTabWidget::TabPosition pos)
{
    Q_D(QxTabWidget);
    if (d->m_pos == pos) {
        return;
    }
    d->m_pos = pos;
    d->updateTabBarPosition();
}

QToolButton *QxTabWidget::addButton(const QString &text)
{
    return addButton(QIcon(), text);
}

QToolButton *QxTabWidget::addButton(const QIcon &icon, const QString &text)
{
    Q_D(QxTabWidget);
    QToolButton *button = new QToolButton(this);
    button->setText(text);
    button->setToolTip(text);
    button->setIcon(icon);
    button->setIconSize(d->m_tabBar->iconSize());
    button->setAutoRaise(true);
    button->setFocusPolicy(Qt::NoFocus);
    button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    d->m_tabLayout->addWidget(button);
    return button;
}

void QxTabWidget::removeButton(QToolButton *button)
{
    if (button == Q_NULLPTR) {
        return;
    }
    Q_D(QxTabWidget);
    d->m_tabLayout->removeWidget(button);
    delete button;
}

void QxTabWidget::setCurrentIndex(int index)
{
    Q_D(QxTabWidget);
    d->m_tabBar->setCurrentIndex(index);
}

void QxTabWidget::setCurrentWidget(QWidget *widget)
{
    Q_D(QxTabWidget);
    d->m_tabBar->setCurrentIndex(indexOf(widget));
}

QX_APP_END_NAMESPACE
