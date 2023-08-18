/**
 * Copyright (C) 2021-2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#include "QxQuickAccessBar.h"
#include "QxQuickAccessBarPrivate.h"

#include <QToolButton>
#include <QAction>
#include <QMenu>
#include <QStyle>
#include <QEvent>
#include <QActionEvent>
#include <QLayout>

QX_BEGIN_NAMESPACE

/* QuickAccessButton */
QuickAccessButton::QuickAccessButton(QWidget *parent)
    : QToolButton(parent)
    , m_orientation(Qt::Horizontal)
{
    setObjectName(QLatin1String("qx_QuickAccessButton"));
}

QuickAccessButton::~QuickAccessButton()
{

}

QSize QuickAccessButton::sizeHint() const
{
    QSize sz = QToolButton::sizeHint();
    if (m_orientation == Qt::Horizontal) {
        return QSize(sz.width() / 2, sz.height());
    } else {
        return QSize(sz.width(), sz.height() / 2);
    }
}

void QuickAccessButton::setOrientation(Qt::Orientation orientation)
{
    m_orientation = orientation;
}

/* QuickAccessBarPrivate */
QuickAccessBarPrivate::QuickAccessBarPrivate()
    : m_menu(Q_NULLPTR)
    , m_actionAccessPopup(Q_NULLPTR)
    , m_customizeGroup(Q_NULLPTR)
    , m_accessPopup(Q_NULLPTR)
    , m_removingAction(false)
{
}

QuickAccessBarPrivate::~QuickAccessBarPrivate()
{

}

void QuickAccessBarPrivate::init()
{
    Q_Q(QuickAccessBar);
    q->setAutoFillBackground(false);
    q->setAttribute(Qt::WA_NoSystemBackground);
    q->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    const int sz = q->style()->pixelMetric(QStyle::PM_SmallIconSize, Q_NULLPTR, q);
    q->setIconSize(QSize(sz, sz));

    m_accessPopup = new QuickAccessButton(q);
    m_accessPopup->setPopupMode(QToolButton::InstantPopup);
    connect(q, SIGNAL(orientationChanged(Qt::Orientation)), m_accessPopup, SLOT(setOrientation(Qt::Orientation)));
    m_menu = new QMenu(q);
    m_accessPopup->setMenu(m_menu);

    m_actionAccessPopup = q->addWidget(m_accessPopup);

    m_customizeGroup = new QActionGroup(q);
    m_customizeGroup->setExclusive(false);
    connect(m_customizeGroup, SIGNAL(triggered(QAction *)), this, SLOT(customizeAction(QAction *)));

    connect(m_menu, SIGNAL(aboutToShow()), this, SLOT(aboutToShowCustomizeMenu()));
    connect(m_menu, SIGNAL(aboutToHide()), this, SLOT(aboutToHideCustomizeMenu()));
}

QuickAccessAction *QuickAccessBarPrivate::findQuickAccessAction(QAction *action) const
{
    if (m_customizeGroup == Q_NULLPTR) {
        return Q_NULLPTR;
    }

    QList<QAction *> list = m_customizeGroup->actions();
    for (int i = 0; i < list.count(); ++i) {
        QuickAccessAction *act = dynamic_cast<QuickAccessAction*>(list[i]);
        if (act && action == act->m_srcAction) {
            return act;
        }
    }
    return Q_NULLPTR;
}

void QuickAccessBarPrivate::updateAction(QAction *action)
{
    if (QuickAccessAction* wrapper = findQuickAccessAction(action)) {
        wrapper->update();
    }
}

void QuickAccessBarPrivate::setActionVisible(QAction *action, bool visible)
{
    if (QuickAccessAction *wrapper = findQuickAccessAction(action)) {
        setActionVisible(wrapper, action, visible);
    }
}

void QuickAccessBarPrivate::setActionVisible(QuickAccessAction *wrapper, QAction *action, bool visible)
{
    action->setVisible(visible);
    wrapper->update();
}

void QuickAccessBarPrivate::customizeAction(QAction *action)
{
    Q_Q(QuickAccessBar);
    if (QuickAccessAction *act = dynamic_cast<QuickAccessAction*>(action)) {
        act->m_srcAction->setVisible(act->isChecked());
        emit q->customizeActionChanged();
    }
}

void QuickAccessBarPrivate::aboutToShowCustomizeMenu()
{
    Q_Q(QuickAccessBar);
    m_menu->clear();
    foreach (QAction *action, m_actionList) {
        m_menu->addAction(action);
    }
    emit q->showCustomizeMenu(m_menu);
}

void QuickAccessBarPrivate::aboutToHideCustomizeMenu()
{
    m_menu->clear();
}

/* QuickAccessBar */
QuickAccessBar::QuickAccessBar(QWidget *parent)
    : QToolBar(parent)
{
    QX_INIT_PRIVATE(QuickAccessBar)
    Q_D(QuickAccessBar);
    d->init();
    layout()->setSizeConstraint(QLayout::SetFixedSize);
}

QuickAccessBar::~QuickAccessBar()
{
    QX_FINI_PRIVATE()
}

QAction *QuickAccessBar::actionCustomizeButton() const
{
    Q_D(const QuickAccessBar);
    return d->m_actionAccessPopup;
}

void QuickAccessBar::setActionVisible(QAction *action, bool visible)
{
    Q_D(QuickAccessBar);
    d->setActionVisible(action, visible);
}

bool QuickAccessBar::isActionVisible(QAction *action) const
{
    Q_D(const QuickAccessBar);
    if (QuickAccessAction *wrapper = d->findQuickAccessAction(action)) {
        return wrapper->isChecked();
    }
    return false;
}

int QuickAccessBar::visibleCount() const
{
    Q_D(const QuickAccessBar);
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

QByteArray QuickAccessBar::state() const
{
    Q_D(const QuickAccessBar);
    QByteArray s;
    foreach (QAction *act, d->m_actionList) {
        s.append(act->isChecked() ? '1' : '0');
    }
    return s;
}

void QuickAccessBar::setState(const QByteArray &s)
{
    if (s.isEmpty()) {
        return;
    }
    Q_D(QuickAccessBar);
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

bool QuickAccessBar::event(QEvent *event)
{
    if (parentWidget() && (event->type() == QEvent::Hide || event->type() == QEvent::Show)) {
        adjustSize();
    } else if (event->type() == QEvent::StyleChange) {
        const int sz = style()->pixelMetric(QStyle::PM_SmallIconSize, Q_NULLPTR, this);
        setIconSize(QSize(sz, sz));
    }
    return QToolBar::event(event);
}

void QuickAccessBar::actionEvent(QActionEvent *event)
{
    Q_D(QuickAccessBar);
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

QX_END_NAMESPACE
