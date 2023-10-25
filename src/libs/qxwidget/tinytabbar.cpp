/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#include "tinytabbar.h"
#include "tinytabbar_p.h"

#include <QAction>
#include <QWidgetAction>
#include <QActionGroup>
#include <QActionEvent>
#include <QLayout>
#include <QToolButton>
#include <QDebug>

QX_WIDGET_BEGIN_NAMESPACE


TinyTabBarPrivate::TinyTabBarPrivate()
    : m_currentIndex(-1)
    , m_togglable(true)
{

}

TinyTabBarPrivate::~TinyTabBarPrivate()
{

}

void TinyTabBarPrivate::init()
{
    Q_Q(TinyTabBar);
    q->layout()->setSpacing(0);
    q->layout()->setContentsMargins(0, 0, 0, 0);

    m_group = new QActionGroup(q);
    m_group->setExclusionPolicy(QActionGroup::ExclusionPolicy::ExclusiveOptional);
    connect(m_group, &QActionGroup::triggered, this, &TinyTabBarPrivate::onTriggered);
}

int TinyTabBarPrivate::indexOf(QAction *action)
{
    for (int i = 0; i < m_tabs.count(); ++i) {
        if (m_tabs.at(i) == action) {
            return i;
        }
    }
    return -1;
}

void TinyTabBarPrivate::onTriggered(QAction *action)
{
    Q_Q(TinyTabBar);
    int index = indexOf(action);
    if (index != m_currentIndex) {
        m_currentIndex = index;
        emit q->currentChanged(index);
    }
    if (m_togglable) {
        emit q->currentToggled(index, action->isChecked());
    }
}

TinyTabBar::TinyTabBar(QWidget *parent)
    : QToolBar{parent}
{
    QX_INIT_PRIVATE(TinyTabBar)
    Q_D(TinyTabBar);
    d->init();
}

TinyTabBar::TinyTabBar(TinyTabBarPrivate *dd, QWidget *parent)
    : QToolBar{parent}
{
    QX_SET_PRIVATE(dd)
    Q_D(TinyTabBar);
    d->init();
}

TinyTabBar::~TinyTabBar()
{
    QX_FINI_PRIVATE()
}

int TinyTabBar::addTab(const QString &text)
{
    return addTab(QIcon(), text);
}

int TinyTabBar::addTab(const QIcon &icon, const QString &text)
{
    return insertTab(-1, icon, text);
}

int TinyTabBar::insertTab(int index, const QString &text)
{
    return insertTab(index, QIcon(), text);
}

int TinyTabBar::insertTab(int index, const QIcon &icon, const QString &text)
{
    Q_D(TinyTabBar);
    QWidgetAction *action = new QWidgetAction(this);
    action->setIcon(icon);
    action->setText(text);
    action->setCheckable(true);

    QToolButton *button = new QToolButton(this);
    button->setAutoRaise(true);
    button->setFocusPolicy(Qt::NoFocus);
    button->setDefaultAction(action);
    button->setToolButtonStyle(toolButtonStyle());
    button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    QObject::connect(this, SIGNAL(iconSizeChanged(QSize)),
                     button, SLOT(setIconSize(QSize)));
    QObject::connect(this, SIGNAL(toolButtonStyleChanged(Qt::ToolButtonStyle)),
                     button, SLOT(setToolButtonStyle(Qt::ToolButtonStyle)));
    QObject::connect(button, SIGNAL(triggered(QAction*)), this, SIGNAL(actionTriggered(QAction*)));

    action->setDefaultWidget(button);

    if (!d->validIndex(index)) {
        index = d->m_tabs.count();
        // add to list first, so that inheritance class knows whether to append or insert
        d->m_tabs.append(action);
        addAction(action);
    } else {
        QAction *beforeAction = d->m_tabs.at(index);
        d->m_tabs.insert(index, action);
        insertAction(beforeAction, action);
    }
    d->m_group->addAction(action);

    if (d->m_tabs.count() == 1) {
        setCurrentIndex(index);
    } else if (index <= d->m_currentIndex) {
        ++d->m_currentIndex;
    }

    return index;
}

void TinyTabBar::removeTab(int index)
{
    Q_D(TinyTabBar);
    if (!d->validIndex(index)) {
        return;
    }
    QAction *action = d->m_tabs.takeAt(index);
    removeAction(action);
    d->m_group->removeAction(action);
    delete action;

    int newIndex = -1;
    if (index == d->m_currentIndex) {
        d->m_currentIndex = -1;
        int newCnt = d->m_tabs.count();
        if (newCnt > 0) {
            newIndex = index;
            if (newIndex >= newCnt) {
                newIndex = newCnt - 1;
            }
            setCurrentIndex(newIndex);
        }
    } else if (index < d->m_currentIndex) {
        newIndex = d->m_currentIndex - 1;
        d->m_currentIndex = -1;
        setCurrentIndex(newIndex);
    }
    // empty or setCurrentIndex failed
    if (d->m_currentIndex == -1) {
        emit currentChanged(-1);
    }
}

void TinyTabBar::setTabEnabled(int index, bool enable)
{
    Q_D(TinyTabBar);
    if (d->validIndex(index)) {
        d->m_tabs[index]->setEnabled(enable);
    }
}

bool TinyTabBar::isTabEnabled(int index) const
{
    Q_D(const TinyTabBar);
    if (d->validIndex(index)) {
        return d->m_tabs.at(index)->isEnabled();
    }
    return false;
}

void TinyTabBar::setTabVisible(int index, bool visible)
{
    Q_D(TinyTabBar);
    if (d->validIndex(index)) {
        d->m_tabs.at(index)->setVisible(visible);
    }
}

bool TinyTabBar::isTabVisible(int index) const
{
    Q_D(const TinyTabBar);
    if (d->validIndex(index)) {
        return d->m_tabs.at(index)->isVisible();
    }
    return false;
}

void TinyTabBar::setTabToolTip(int index, const QString &toolTip)
{
    Q_D(TinyTabBar);
    if (d->validIndex(index)) {
        d->m_tabs[index]->setToolTip(toolTip);
    }
}

QString TinyTabBar::tabToolTip(int index) const
{
    Q_D(const TinyTabBar);
    if (d->validIndex(index)) {
        return d->m_tabs.at(index)->toolTip();
    }
    return QString();
}

bool TinyTabBar::isTabChecked(int index) const
{
    Q_D(const TinyTabBar);
    if (d->validIndex(index)) {
        return d->m_tabs.at(index)->isChecked();
    }
    return false;
}

void TinyTabBar::setCurrentIndex(int index)
{
    Q_D(TinyTabBar);
    if (d->m_currentIndex != index && isTabEnabled(index)) {
        // update m_currentIndex in onTriggered
        d->m_tabs.at(index)->trigger();
    }
}

int TinyTabBar::currentIndex() const
{
    Q_D(const TinyTabBar);
    return d->m_currentIndex;
}

void TinyTabBar::setTogglable(bool able)
{
    Q_D(TinyTabBar);
    if (d->m_togglable == able) {
        return;
    }
    d->m_togglable = able;
    d->m_group->setExclusionPolicy(able ? QActionGroup::ExclusionPolicy::ExclusiveOptional
                                        : QActionGroup::ExclusionPolicy::Exclusive);
}

bool TinyTabBar::isTogglable() const
{
    Q_D(const TinyTabBar);
    return d->m_togglable;
}

void TinyTabBar::setTabStyle(Qt::ToolButtonStyle style)
{
    setToolButtonStyle(style);
}

int TinyTabBar::count() const
{
    Q_D(const TinyTabBar);
    return d->m_tabs.count();
}

void TinyTabBar::actionEvent(QActionEvent *event)
{
    QToolBar::actionEvent(event);
}

QX_WIDGET_END_NAMESPACE
