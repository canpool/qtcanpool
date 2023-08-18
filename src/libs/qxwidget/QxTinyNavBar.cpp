/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#include "QxTinyNavBar.h"
#include "QxTinyNavBarPrivate.h"
#include "QxQuickAccessBarPrivate.h"

#include <QMenu>
#include <QAction>
#include <QActionEvent>
#include <QStyle>

QX_BEGIN_NAMESPACE


TinyNavBarPrivate::TinyNavBarPrivate()
    : TinyTabBarPrivate()
    , m_actionAccessPopup(Q_NULLPTR)
    , m_removingAction(false)
{
    m_actionMap.clear();
}

void TinyNavBarPrivate::init()
{
    Q_Q(TinyNavBar);
    q->setAutoFillBackground(false);
    q->setAttribute(Qt::WA_NoSystemBackground);
    q->setTabStyle(Qt::ToolButtonTextOnly);

    const int sz = q->style()->pixelMetric(QStyle::PM_SmallIconSize, Q_NULLPTR, q);
    q->setIconSize(QSize(sz, sz));

    m_accessPopup = new QuickAccessButton(q);
    m_accessPopup->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_accessPopup->setPopupMode(QToolButton::InstantPopup);
    connect(q, SIGNAL(orientationChanged(Qt::Orientation)), m_accessPopup, SLOT(setOrientation(Qt::Orientation)));
    m_menu = new QMenu(q);
    m_accessPopup->setMenu(m_menu);

    m_actionAccessPopup = q->addWidget(m_accessPopup);

    m_customizeGroup = new QActionGroup(q);
    m_customizeGroup->setExclusive(false);
    connect(m_customizeGroup, SIGNAL(triggered(QAction*)), this, SLOT(customizeAction(QAction*)));

    connect(m_menu, SIGNAL(aboutToShow()), this, SLOT(aboutToShowCustomizeMenu()));
    connect(m_menu, SIGNAL(aboutToHide()), this, SLOT(aboutToHideCustomizeMenu()));
}

void TinyNavBarPrivate::customizeAction(QAction *action)
{
    Q_Q(TinyNavBar);
    if (QAction *lowAction = m_actionMap.key(action)) {
        lowAction->setVisible(action->isChecked());
        emit q->customizeTabChanged();
    }
}

void TinyNavBarPrivate::aboutToShowCustomizeMenu()
{
    Q_Q(TinyNavBar);
    m_menu->clear();
    foreach (QAction *action, m_actionList) {
        m_menu->addAction(action);
    }
    emit q->showCustomizeMenu(m_menu);
}

void TinyNavBarPrivate::aboutToHideCustomizeMenu()
{
    m_menu->clear();
}


TinyNavBar::TinyNavBar(QWidget *parent)
    : TinyTabBar(new TinyNavBarPrivate(), parent)
{
    Q_D(TinyNavBar);
    d->setPublic(this);
    d->init();
}

TinyNavBar::~TinyNavBar()
{
    // d will be detroy in ~TinyTabBar()
}

QAction *TinyNavBar::actionCustomizeButton() const
{
    Q_D(const TinyNavBar);
    return d->m_actionAccessPopup;
}

int TinyNavBar::visibleCount() const
{
    Q_D(const TinyNavBar);
    int vcnt = 0;
    foreach (QAction *act, d->m_actionList) {
        if (act->isChecked()) {
            ++vcnt;
        }
    }
    return vcnt;
}

void TinyNavBar::actionEvent(QActionEvent *event)
{
    TinyTabBar::actionEvent(event);
    Q_D(TinyNavBar);
    QAction *lowAction = event->action();
    if (d->m_actionAccessPopup)  {
        if (event->type() == QEvent::ActionAdded) {
            if (!d->m_removingAction) {
                // remove, then add back in QEvent::ActionRemoved
                removeAction(d->m_actionAccessPopup);
            }
            QAction *checkAction = d->m_actionMap.value(lowAction);
            if (lowAction != d->m_actionAccessPopup && !checkAction) {
                QToolButton *button = qobject_cast<QToolButton *>(widgetForAction(lowAction));
                if (button) {
                    int curIndex = d->m_tabs.indexOf(button);
                    int cnt = d->m_tabs.count();
                    QAction *act = new QAction(button->text(), this);
                    act->setCheckable(true);
                    act->setChecked(lowAction->isVisible());
                    if (curIndex + 1 == cnt) {
                        d->m_actionList.append(act);
                    } else {
                        d->m_actionList.insert(curIndex, act);
                    }
                    d->m_customizeGroup->addAction(act);
                    d->m_actionMap[lowAction] = act;
                    adjustSize();
                }
            } else if (checkAction) {
                checkAction->setChecked(lowAction->isVisible());
            }
        } else if (event->type() == QEvent::ActionRemoved) {
            if (lowAction == d->m_actionAccessPopup) {
                d->m_removingAction = true;
                addAction(d->m_actionAccessPopup);
                d->m_removingAction = false;
            } else {
                QAction *act = d->m_actionMap.value(lowAction);
                if (act) {
                    d->m_actionList.removeOne(act);
                    d->m_customizeGroup->removeAction(act);
                    d->m_actionMap.remove(lowAction);
                    delete act;
                }
            }
        } else if (event->type() == QEvent::ActionChanged) {
            if (lowAction == d->m_actionAccessPopup) {
                d->m_accessPopup->setDefaultAction(d->m_actionAccessPopup);
            } else if (QAction *checkAction = d->m_actionMap.value(lowAction)) {
                checkAction->setChecked(lowAction->isVisible());
            }
        }
    }
}

QX_END_NAMESPACE
