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

QCANPOOL_BEGIN_NAMESPACE


TinyTabBarPrivate::TinyTabBarPrivate()
    : m_currentIndex(-1)
    , m_togglable(true)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    m_togglable = false;
#endif
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
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    m_group->setExclusive(true);
#else
    m_group->setExclusionPolicy(QActionGroup::ExclusionPolicy::ExclusiveOptional);
#endif
    connect(m_group, &QActionGroup::triggered, this, &TinyTabBarPrivate::onTriggered);

    connect(q, &TinyTabBar::orientationChanged, this, &TinyTabBarPrivate::onOrientationChanged);
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

void TinyTabBarPrivate::layoutActions()
{
    Q_Q(TinyTabBar);
    if (q->orientation() == Qt::Vertical) {
        int w = 0;
        foreach (QAction *act, m_tabs) {
            int aw = q->widgetForAction(act)->sizeHint().width();
            if (w < aw) w = aw;
        }
        foreach (QAction *act, m_tabs) {
            q->widgetForAction(act)->setFixedWidth(w);
        }
    } else {
        foreach (QAction *act, m_tabs) {
            QWidget *widget = q->widgetForAction(act);
            widget->setFixedWidth(widget->sizeHint().width());
        }
    }
}

void TinyTabBarPrivate::onTriggered(QAction *action)
{
    Q_Q(TinyTabBar);
    int index = indexOf(action);
    if (index != m_currentIndex) {
        m_currentIndex = index;
        Q_EMIT q->currentChanged(index);
    }
    if (m_togglable) {
        Q_EMIT q->currentToggled(index, action->isChecked());
    }
}

void TinyTabBarPrivate::onOrientationChanged(Qt::Orientation orientation)
{
    Q_UNUSED(orientation);
    layoutActions();
}

TinyTabBar::TinyTabBar(QWidget *parent)
    : QToolBar{parent}
{
    QCP_INIT_PRIVATE(TinyTabBar)
    Q_D(TinyTabBar);
    d->init();
}

TinyTabBar::TinyTabBar(TinyTabBarPrivate *d, QWidget *parent)
    : QToolBar{parent}
{
    QCP_SET_PRIVATE(d)
    d->init();
}

TinyTabBar::~TinyTabBar()
{
    QCP_FINI_PRIVATE()
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
    QAction *action = new QAction(this);
    action->setIcon(icon);
    action->setText(text);
    action->setCheckable(true);

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

    d->layoutActions();

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
        Q_EMIT currentChanged(-1);
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
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    Q_UNUSED(able);
#else
    Q_D(TinyTabBar);
    if (d->m_togglable == able) {
        return;
    }
    d->m_togglable = able;
    d->m_group->setExclusionPolicy(able ? QActionGroup::ExclusionPolicy::ExclusiveOptional
                                        : QActionGroup::ExclusionPolicy::Exclusive);
#endif
}

bool TinyTabBar::isTogglable() const
{
    Q_D(const TinyTabBar);
    return d->m_togglable;
}

void TinyTabBar::setTabStyle(Qt::ToolButtonStyle style)
{
    Q_D(TinyTabBar);
    setToolButtonStyle(style);
    // If layoutActions are called in the slot function of toolButtonStyleChanged,
    // the size of Actions cannot be updated immediately (the size after the last
    // format change is retrieved at the next format change)
    d->layoutActions();
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

QCANPOOL_END_NAMESPACE
