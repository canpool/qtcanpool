/**
 * Copyright (C) 2018-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#include "fancytabbar_p.h"
#include "fancytabbar.h"

#include <QToolButton>
#include <QMap>
#include <QBoxLayout>
#include <QAction>
#include <QResizeEvent>

QCANPOOL_BEGIN_NAMESPACE

FancyTabBarPrivate::FancyTabBarPrivate()
    : m_currentIndex(-1)
    , m_spacing(2)
    , m_orientation(Qt::Horizontal)
    , m_iconSize(QSize(22, 22))
    , q(nullptr)
{
}

FancyTabBarPrivate::~FancyTabBarPrivate()
{
    qDeleteAll(m_tabs);
    m_tabs.clear();
    qDeleteAll(m_actionButtons);
    m_actionButtons.clear();
    m_actionTabMap.clear();
}

void FancyTabBarPrivate::init()
{
    QBoxLayout::Direction direction = layoutDirection();

    m_frontActionLayout = new QBoxLayout(direction);
    m_frontActionLayout->setContentsMargins(0, 0, 0, 0);
    m_frontActionLayout->setSpacing(m_spacing);

    m_middleActionLayout = new QBoxLayout(direction);
    m_middleActionLayout->setContentsMargins(0, 0, 0, 0);
    m_middleActionLayout->setSpacing(m_spacing);

    m_backActionLayout = new QBoxLayout(direction);
    m_backActionLayout->setContentsMargins(0, 0, 0, 0);
    m_backActionLayout->setSpacing(m_spacing);

    m_tabLayout = new QBoxLayout(direction);
    m_tabLayout->setContentsMargins(0, 0, 0, 0);
    m_tabLayout->setSpacing(m_spacing);

    m_mainLayout = new QBoxLayout(direction);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(1);
    m_mainLayout->addLayout(m_frontActionLayout);
    m_mainLayout->addLayout(m_tabLayout);
    m_mainLayout->addLayout(m_middleActionLayout);
    m_mainLayout->addStretch();
    m_mainLayout->addLayout(m_backActionLayout);

    q->setLayout(m_mainLayout);
}

QToolButton *FancyTabBarPrivate::at(int index)
{
    return validIndex(index) ? m_tabs[index] : nullptr;
}

void FancyTabBarPrivate::setIconSize(QSize size)
{
    QList<QToolButton *> tabButtons = q->findChildren<QToolButton *>();
    foreach (QToolButton *tab, tabButtons) {
        tab->setIconSize(size);
    }
    m_iconSize = size;
}

void FancyTabBarPrivate::switchTab(bool checked)
{
    QToolButton *tab = qobject_cast<QToolButton*>(sender());
    if (tab == nullptr) return;
    if (!checked) {
        tab->setChecked(true);
        return;
    }
    int index = m_tabs.indexOf(tab);
    q->setCurrentIndex(index);
}

void FancyTabBarPrivate::pressTab()
{
    QToolButton *tab = qobject_cast<QToolButton*>(sender());
    if (tab == nullptr) return;
    int index = m_tabs.indexOf(tab);
    q->setCurrentIndex(index);
}

FancyTabBar::FancyTabBar(QWidget *parent)
    : QWidget(parent)
    , d(new FancyTabBarPrivate())
{
    setAttribute(Qt::WA_StyledBackground, true);
    setObjectName(QLatin1String("qtc_tabbar"));

    d->q = this;
    d->init();
}

FancyTabBar::~FancyTabBar()
{
    delete d;
}

int FancyTabBar::addTab(const QString &text)
{
    return insertTab(-1, text);
}

int FancyTabBar::addTab(const QIcon &icon, const QString &text)
{
    return insertTab(-1, icon, text);
}

int FancyTabBar::insertTab(int index, const QString &text)
{
    return insertTab(index, QIcon(), text);
}

int FancyTabBar::insertTab(int index, const QIcon &icon, const QString &text)
{
    QToolButton *tab = new QToolButton();
    tab->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    tab->setObjectName(QLatin1String("tab"));
    tab->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    tab->setText(text);
    tab->setToolTip(text);
    tab->setIcon(icon);
    tab->setIconSize(d->m_iconSize);
    tab->setCheckable(true);
    connect(tab, SIGNAL(clicked(bool)), d, SLOT(switchTab(bool)));
    connect(tab, SIGNAL(pressed()), d, SLOT(pressTab()));

    if (d->validIndex(index)){
        d->m_tabs.insert(index, tab);
        d->m_tabLayout->insertWidget(index, tab);
    } else {
        index = d->m_tabs.count();
        d->m_tabs.append(tab);
        d->m_tabLayout->addWidget(tab);
    }

    if (d->m_tabs.count() == 1) {
        setCurrentIndex(index);
    } else if (d->m_currentIndex >= index) {
        d->m_currentIndex++;
    }
    return index;
}

void FancyTabBar::removeTab(int index)
{
    if (!d->validIndex(index)) return;

    QToolButton *tab = d->m_tabs.takeAt(index);
    d->m_tabLayout->removeWidget(tab);
    delete tab;

    int count = d->m_tabs.count();
    if (count == 0) {
        d->m_currentIndex = -1;
    } else if (index < d->m_currentIndex) {
        d->m_currentIndex--;
    } else if (index == d->m_currentIndex) {
        d->m_currentIndex = -1;
        if(index == count) {
            index--;
        }
        setCurrentIndex(index);
    }
}

int FancyTabBar::currentIndex() const
{
    if (d->validIndex(d->m_currentIndex)) {
        return d->m_currentIndex;
    }
    return -1;
}

int FancyTabBar::count() const
{
    return d->m_tabs.count();
}

bool FancyTabBar::isTabEnabled(int index) const
{
    if (QToolButton *tab = d->at(index)) {
        return tab->isEnabled();
    }
    return false;
}

void FancyTabBar::setTabEnabled(int index, bool enable)
{
    if (QToolButton *tab = d->at(index)) {
        tab->setEnabled(enable);
        if (!enable && index == d->m_currentIndex) {
            setCurrentIndex(d->validIndex(index + 1) ? (index + 1) : 0);
        } else if (enable && !d->validIndex(d->m_currentIndex)) {
            setCurrentIndex(index);
        }
    }
}

bool FancyTabBar::isTabVisible(int index) const
{
    if (QToolButton *tab = d->at(index)) {
        return tab->isVisible();
    }
    return false;
}

void FancyTabBar::setTabVisible(int index, bool visible)
{
    if (QToolButton *tab = d->at(index)) {
        tab->setVisible(visible);
        if (!visible && index == d->m_currentIndex) {
            setCurrentIndex(d->validIndex(index + 1) ? (index + 1) : 0);
        } else if (visible && !d->validIndex(d->m_currentIndex)) {
            setCurrentIndex(index);
        }
    }
}

QString FancyTabBar::tabText(int index) const
{
    if (QToolButton *tab = d->at(index)) {
        return tab->text();
    }
    return QString();
}

void FancyTabBar::setTabText(int index, const QString &text)
{
    if (QToolButton *tab = d->at(index)) {
        return tab->setText(text);
    }
}

QIcon FancyTabBar::tabIcon(int index) const
{
    if (QToolButton *tab = d->at(index)) {
        return tab->icon();
    }
    return QIcon();
}

void FancyTabBar::setTabIcon(int index, const QIcon &icon)
{
    if (QToolButton *tab = d->at(index)) {
        return tab->setIcon(icon);
    }
}

#ifndef QT_NO_TOOLTIP
void FancyTabBar::setTabToolTip(int index, const QString &tip)
{
    if (QToolButton *tab = d->at(index)) {
        return tab->setToolTip(tip);
    }
}

QString FancyTabBar::tabToolTip(int index) const
{
    if (QToolButton *tab = d->at(index)) {
        return tab->toolTip();
    }
    return QString();
}

#endif

void FancyTabBar::setTabIconsOnly(bool iconOnly)
{
    Qt::ToolButtonStyle style = iconOnly ? Qt::ToolButtonIconOnly :
                                           Qt::ToolButtonTextUnderIcon;
    foreach (QToolButton *tab, d->m_tabs) {
        tab->setToolButtonStyle(style);
    }
}

QAction *FancyTabBar::addAction(const QIcon &icon, const QString &text,
                                FancyTabBar::ActionPosition position)
{
    QAction *action = new QAction(icon, text);
    addAction(action, position);
    return action;
}

void FancyTabBar::addAction(QAction *action, FancyTabBar::ActionPosition position)
{
    QToolButton *button = new QToolButton();
    button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    button->setObjectName(QLatin1String("button"));
    button->setToolButtonStyle(Qt::ToolButtonIconOnly);
    button->setText(action->text());
    button->setToolTip(action->toolTip());
    button->setIconSize(d->m_iconSize);
    button->setDefaultAction(action);
    d->m_actionButtons.append(button);
    d->m_actionTabMap.insert(action, button);

    switch (position) {
        case FancyTabBar::Front: {
            d->m_frontActionLayout->addWidget(button);
            break;
        }
        case FancyTabBar::Middle: {
            d->m_middleActionLayout->addWidget(button);
            break;
        }
        case FancyTabBar::Back: {
            d->m_backActionLayout->addWidget(button);
            break;
        }
    }
}

void FancyTabBar::addActionButton(QToolButton *button, FancyTabBar::ActionPosition position)
{
    button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    button->setObjectName(QLatin1String("button"));
    button->setIconSize(d->m_iconSize);
    d->m_actionButtons.append(button);
    QAction *action = button->defaultAction();
    if (action) {
        d->m_actionTabMap.insert(action, button);
    }

    switch (position) {
        case FancyTabBar::Front: {
            d->m_frontActionLayout->addWidget(button);
            break;
        }
        case FancyTabBar::Middle: {
            d->m_middleActionLayout->addWidget(button);
            break;
        }
        case FancyTabBar::Back: {
            d->m_backActionLayout->addWidget(button);
            break;
        }
    }
}

void FancyTabBar::setActionIconOnly(QAction *action, bool iconOnly)
{
    Qt::ToolButtonStyle style = iconOnly ? Qt::ToolButtonIconOnly :
                                           Qt::ToolButtonTextUnderIcon;
    if (QToolButton *button = d->m_actionTabMap.value(action)) {
        button->setToolButtonStyle(style);
    }
}

void FancyTabBar::setSpacing(int spacing)
{
    if (d->m_spacing == spacing)
        return;
    d->m_spacing = spacing;
    d->m_frontActionLayout->setSpacing(spacing);
    d->m_middleActionLayout->setSpacing(spacing);
    d->m_backActionLayout->setSpacing(spacing);
    d->m_tabLayout->setSpacing(spacing);
}

void FancyTabBar::setIconSize(QSize size)
{
    d->setIconSize(size);
}

Qt::Orientation FancyTabBar::orientation() const
{
    return d->m_orientation;
}

void FancyTabBar::setOrientation(Qt::Orientation orientation)
{
    if (d->m_orientation == orientation) {
        return;
    }
    d->m_orientation = orientation;
    QBoxLayout::Direction layoutDirection = d->layoutDirection();
    d->m_frontActionLayout->setDirection(layoutDirection);
    d->m_middleActionLayout->setDirection(layoutDirection);
    d->m_backActionLayout->setDirection(layoutDirection);
    d->m_tabLayout->setDirection(layoutDirection);
    d->m_mainLayout->setDirection(layoutDirection);
    emit orientationChanged(orientation);
}

void FancyTabBar::setCurrentIndex(int index)
{
    if (index == d->m_currentIndex) return;
    if (d->m_currentIndex != -1) {
        d->m_tabs.at(d->m_currentIndex)->setChecked(false);
    }
    d->m_currentIndex = index;
    d->m_tabs.at(d->m_currentIndex)->setChecked(true);
    emit currentChanged(index);
}

QCANPOOL_END_NAMESPACE
