/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#include "tinynavbar.h"
#include "tinynavbar_p.h"
#include "menuaccessbutton.h"

#include <QMenu>
#include <QAction>
#include <QActionEvent>
#include <QActionGroup>
#include <QStyle>

QCANPOOL_BEGIN_NAMESPACE

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

    const int sz = q->style()->pixelMetric(QStyle::PM_SmallIconSize, Q_NULLPTR, q);
    q->setIconSize(QSize(sz, sz));

    m_accessPopup = new MenuAccessButton(q);
    connect(q, &TinyNavBar::orientationChanged, m_accessPopup, &MenuAccessButton::setOrientation);
    connect(q, &TinyNavBar::toolButtonStyleChanged, m_accessPopup, &MenuAccessButton::setToolButtonStyle);
    m_menu = new QMenu(q);
    m_accessPopup->setMenu(m_menu);

    m_actionAccessPopup = q->addWidget(m_accessPopup);

    m_customizeGroup = new QActionGroup(q);
    m_customizeGroup->setExclusive(false);
    connect(m_customizeGroup, SIGNAL(triggered(QAction*)), this, SLOT(customizeAction(QAction*)));

    connect(m_menu, SIGNAL(aboutToShow()), this, SLOT(aboutToShowCustomizeMenu()));
    connect(m_menu, SIGNAL(aboutToHide()), this, SLOT(aboutToHideCustomizeMenu()));

    q->setAutoFillBackground(false);
    q->setAttribute(Qt::WA_NoSystemBackground);
    q->setTabStyle(Qt::ToolButtonTextOnly);
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
    setObjectName(QLatin1String("qtc_tinynavbar"));
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
                int curIndex = d->m_tabs.indexOf(lowAction);
                int cnt = d->m_tabs.count();
                QAction *act = new QAction(lowAction->text(), this);
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

QCANPOOL_END_NAMESPACE
