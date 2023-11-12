/**
 * Copyright (C) 2021-2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "ribbonquickaccessbar_p.h"
#include "ribbonquickaccessbar.h"

#include <QToolButton>
#include <QAction>
#include <QStyle>
#include <QEvent>
#include <QActionEvent>
#include <QActionGroup>
#include <QLayout>

QX_RIBBON_BEGIN_NAMESPACE

RibbonQuickAccessButton::RibbonQuickAccessButton(QWidget *parent)
    : QToolButton(parent)
    , m_orientation(Qt::Horizontal)
{
}

RibbonQuickAccessButton::~RibbonQuickAccessButton()
{

}

QSize RibbonQuickAccessButton::sizeHint() const
{
    QSize sz = QToolButton::sizeHint();
    if (m_orientation == Qt::Horizontal) {
        return QSize(sz.width() / 2, sz.height());
    } else {
        return QSize(sz.width(), sz.height() / 2);
    }
}

void RibbonQuickAccessButton::setOrientation(Qt::Orientation orientation)
{
    m_orientation = orientation;
}


RibbonQuickAccessBarPrivate::RibbonQuickAccessBarPrivate()
    : m_menu(Q_NULLPTR)
    , m_actionAccessPopup(Q_NULLPTR)
    , m_customizeGroup(Q_NULLPTR)
    , m_accessPopup(Q_NULLPTR)
    , m_removingAction(false)
{

}

RibbonQuickAccessBarPrivate::~RibbonQuickAccessBarPrivate()
{

}

void RibbonQuickAccessBarPrivate::init()
{
    Q_Q(RibbonQuickAccessBar);
    q->setAutoFillBackground(false);
    q->setAttribute(Qt::WA_NoSystemBackground);
    q->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    const int sz = q->style()->pixelMetric(QStyle::PM_SmallIconSize, Q_NULLPTR, q);
    q->setIconSize(QSize(sz, sz));

    m_accessPopup = new RibbonQuickAccessButton(q);
    m_accessPopup->setPopupMode(QToolButton::InstantPopup);
    QObject::connect(q, SIGNAL(orientationChanged(Qt::Orientation)),
                     m_accessPopup, SLOT(setOrientation(Qt::Orientation)));
    m_menu = new QMenu(q);
    m_accessPopup->setMenu(m_menu);

    m_actionAccessPopup = q->addWidget(m_accessPopup);

    m_customizeGroup = new QActionGroup(q);
    m_customizeGroup->setExclusive(false);
    connect(m_customizeGroup, SIGNAL(triggered(QAction *)), this, SLOT(customizeAction(QAction *)));

    connect(m_menu, SIGNAL(aboutToShow()), this, SLOT(aboutToShowCustomizeMenu()));
    connect(m_menu, SIGNAL(aboutToHide()), this, SLOT(aboutToHideCustomizeMenu()));
}

QuickAccessAction *RibbonQuickAccessBarPrivate::findQuickAccessAction(QAction *action) const
{
    if (m_customizeGroup == Q_NULLPTR)
        return Q_NULLPTR;

    QList<QAction *> list = m_customizeGroup->actions();
    for (int i = 0; i < list.count(); ++i) {
        QuickAccessAction *act = dynamic_cast<QuickAccessAction*>(list[i]);
        if (act && action == act->m_srcAction) {
            return act;
        }
    }
    return Q_NULLPTR;
}

void RibbonQuickAccessBarPrivate::updateAction(QAction *action)
{
    if (QuickAccessAction* wrapper = findQuickAccessAction(action)) {
        wrapper->update();
    }
}

void RibbonQuickAccessBarPrivate::setActionVisible(QAction *action, bool visible)
{
    if (QuickAccessAction *wrapper = findQuickAccessAction(action)) {
        setActionVisible(wrapper, action, visible);
    }
}

void RibbonQuickAccessBarPrivate::setActionVisible(QuickAccessAction *wrapper, QAction *action, bool visible)
{
    action->setVisible(visible);
    wrapper->update();
}

void RibbonQuickAccessBarPrivate::customizeAction(QAction *action)
{
    Q_Q(RibbonQuickAccessBar);
    if (QuickAccessAction *act = dynamic_cast<QuickAccessAction*>(action)) {
        act->m_srcAction->setVisible(act->isChecked());
        emit q->customizeActionChanged();
    }
}

void RibbonQuickAccessBarPrivate::aboutToShowCustomizeMenu()
{
    Q_Q(RibbonQuickAccessBar);
    m_menu->clear();
    foreach (QAction *action, m_actionList) {
        m_menu->addAction(action);
    }
    emit q->showCustomizeMenu(m_menu);
}

void RibbonQuickAccessBarPrivate::aboutToHideCustomizeMenu()
{
    m_menu->clear();
}

RibbonQuickAccessBar::RibbonQuickAccessBar(QWidget *parent)
    : QToolBar(parent)
{
    QX_INIT_PRIVATE(RibbonQuickAccessBar)
    Q_D(RibbonQuickAccessBar);
    d->init();
    layout()->setSizeConstraint(QLayout::SetFixedSize);
}

RibbonQuickAccessBar::~RibbonQuickAccessBar()
{
    QX_FINI_PRIVATE()
}

QAction *RibbonQuickAccessBar::actionCustomizeButton() const
{
    Q_D(const RibbonQuickAccessBar);
    return d->m_actionAccessPopup;
}

void RibbonQuickAccessBar::setActionVisible(QAction *action, bool visible)
{
    Q_D(RibbonQuickAccessBar);
    d->setActionVisible(action, visible);
}

bool RibbonQuickAccessBar::isActionVisible(QAction *action) const
{
    Q_D(const RibbonQuickAccessBar);
    if (QuickAccessAction *wrapper = d->findQuickAccessAction(action)) {
        return wrapper->isChecked();
    }
    return false;
}

int RibbonQuickAccessBar::visibleCount() const
{
    Q_D(const RibbonQuickAccessBar);
    int visibleCount = 0;
    QList<QAction *> list = d->m_customizeGroup->actions();
    for (int i = 0, count = list.count(); i < count; ++i) {
        if (QuickAccessAction *act = dynamic_cast<QuickAccessAction*>(list[i])) {
            if (act->isChecked()) {
                visibleCount++;
            }
        }
    }
    return visibleCount;
}

QByteArray RibbonQuickAccessBar::state() const
{
    Q_D(const RibbonQuickAccessBar);
    QByteArray s;
    foreach (QAction *act, d->m_actionList) {
        s.append(act->isChecked() ? '1' : '0');
    }
    return s;
}

void RibbonQuickAccessBar::setState(const QByteArray &s)
{
    if (s.isEmpty()) {
        return;
    }
    Q_D(RibbonQuickAccessBar);
    int cnt = s.count();
    int j = 0;
    QList<QAction *> list = d->m_customizeGroup->actions();
    for (int i = 0, count = list.count(); i < count; ++i) {
        if (QuickAccessAction *act = dynamic_cast<QuickAccessAction*>(list[i])) {
            if (j < cnt) {
                if (s.at(j) == '1') {
                    d->setActionVisible(act, act->m_srcAction, true);
                } else if (act->isChecked()) {
                    d->setActionVisible(act, act->m_srcAction, false);
                }
            }
            ++j;
        }
    }
}

bool RibbonQuickAccessBar::event(QEvent *event)
{
    if (parentWidget() && (event->type() == QEvent::Hide || event->type() == QEvent::Show)) {
        adjustSize();
    } else if (event->type() == QEvent::StyleChange) {
        const int sz = style()->pixelMetric(QStyle::PM_SmallIconSize, Q_NULLPTR, this);
        setIconSize(QSize(sz, sz));
    }
    return QToolBar::event(event);
}

void RibbonQuickAccessBar::actionEvent(QActionEvent *event)
{
    Q_D(RibbonQuickAccessBar);
    QToolBar::actionEvent(event);
    if (d->m_actionAccessPopup)  {
        if (event->type() == QEvent::ActionAdded) {
            if (!d->m_removingAction) {
                // remove, then add back in QEvent::ActionRemoved
                removeAction(d->m_actionAccessPopup);
            }
            QuickAccessAction *quickAccessAction = d->findQuickAccessAction(event->action());
            if (event->action() != d->m_actionAccessPopup && !quickAccessAction) {
                QuickAccessAction *act = new QuickAccessAction(this, event->action());
                d->m_actionList.append(act);
                d->m_customizeGroup->addAction(act);
                adjustSize();
            } else if (quickAccessAction) {
                quickAccessAction->update();
            }
        } else if (event->type() == QEvent::ActionRemoved) {
            if (event->action() == d->m_actionAccessPopup) {
                d->m_removingAction = true;
                addAction(d->m_actionAccessPopup);
                d->m_removingAction = false;
            } else {
                QuickAccessAction *act = d->findQuickAccessAction(event->action());
                if (act) {
                    d->m_actionList.removeOne(act);
                    d->m_customizeGroup->removeAction(act);
                    delete act;
                }
            }
        } else if (event->type() == QEvent::ActionChanged) {
            if (event->action() == d->m_actionAccessPopup) {
                d->m_accessPopup->setDefaultAction(d->m_actionAccessPopup);
            } else if (!dynamic_cast<QuickAccessAction *>(event->action())) {
                d->updateAction(event->action());
            }
        }
    }
}

QX_RIBBON_END_NAMESPACE
