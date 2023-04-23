/**
 * Copyright (C) 2021-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#include "quickaccessbar_p.h"
#include "quickaccessbar.h"

#include <QToolButton>
#include <QAction>
#include <QStyle>
#include <QEvent>
#include <QActionEvent>
#include <QLayout>

QCANPOOL_BEGIN_NAMESPACE

QuickAccessButton::QuickAccessButton(QWidget *parent)
    : QToolButton(parent)
{
    setObjectName(QLatin1String("qtc_quickaccessbutton"));
}

QuickAccessButton::~QuickAccessButton()
{

}

QSize QuickAccessButton::sizeHint() const
{
    QSize sz = QToolButton::sizeHint();
    return QSize(sz.width() / 2, sz.height());
}


QuickAccessBarPrivate::QuickAccessBarPrivate()
    : m_menu(Q_NULLPTR)
    , m_actionAccessPopup(Q_NULLPTR)
    , m_customizeGroup(Q_NULLPTR)
    , m_accessPopup(Q_NULLPTR)
    , m_removeAction(false)
    , m_customizeAction(false)
{

}

QuickAccessBarPrivate::~QuickAccessBarPrivate()
{

}

void QuickAccessBarPrivate::init()
{
    q->setAutoFillBackground(false);
    q->setAttribute(Qt::WA_NoSystemBackground);
    q->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    const int sz = q->style()->pixelMetric(QStyle::PM_SmallIconSize, Q_NULLPTR, q);
    q->setIconSize(QSize(sz, sz));

    m_accessPopup = new QuickAccessButton(q);
    m_accessPopup->setPopupMode(QToolButton::InstantPopup);
    m_menu = new QMenu(q);
    m_accessPopup->setMenu(m_menu);

    m_actionAccessPopup = q->addWidget(m_accessPopup);

    m_customizeGroup = new QActionGroup(q);
    m_customizeGroup->setExclusive(false);
    QObject::connect(m_customizeGroup, SIGNAL(triggered(QAction *)), q, SLOT(customizeAction(QAction *)));

    QObject::connect(m_menu, SIGNAL(aboutToShow()), q, SLOT(aboutToShowCustomizeMenu()));
    QObject::connect(m_menu, SIGNAL(aboutToHide()), q, SLOT(aboutToHideCustomizeMenu()));
}

QuickAccessAction *QuickAccessBarPrivate::findQuickAccessAction(QAction *action) const
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

QAction *QuickAccessBarPrivate::findBeforeAction(QAction *action) const
{
    QList<QAction *> list = m_customizeGroup->actions();
    bool find = false;
    for (int i = 0, count = list.count(); i < count; ++i) {
        if (find) {
            if (QuickAccessAction *beforeAct = dynamic_cast<QuickAccessAction*>(list[i])) {
                if (beforeAct->isChecked()) {
                    return beforeAct->m_srcAction;
                }
            }
        }
        if (!find && action == list[i]) {
            find = true;
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
    if (visible) {
        if (m_customizeAction) {
            QAction *beforeAct = findBeforeAction(wrapper);
            q->insertAction(beforeAct, action);
        } else {
            q->addAction(action);
        }
    } else {
        q->removeAction(action);
        m_removeAction = false;
    }
    wrapper->update();
    q->adjustSize();
}

QuickAccessBar::QuickAccessBar(QWidget *parent)
    : QToolBar(parent)
    , d(new QuickAccessBarPrivate())
{
    d->q = this;
    d->init();
    setObjectName(QLatin1String("qtc_quickaccessbar"));
    layout()->setSizeConstraint(QLayout::SetFixedSize);
}

QuickAccessBar::~QuickAccessBar()
{
    delete d;
}

QAction *QuickAccessBar::actionCustomizeButton() const
{
    return d->m_actionAccessPopup;
}

void QuickAccessBar::setActionVisible(QAction *action, bool visible)
{
    d->setActionVisible(action, visible);
}

bool QuickAccessBar::isActionVisible(QAction *action) const
{
    if (QuickAccessAction *wrapper = d->findQuickAccessAction(action)) {
        return wrapper->isChecked();
    }
    return false;
}

int QuickAccessBar::visibleCount() const
{
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
    QByteArray s;
    foreach (QAction *act, d->m_actionList) {
        if (act->isChecked()) {
            s.append('1');
        } else {
            s.append('0');
        }
    }
    return s;
}

void QuickAccessBar::setState(const QByteArray &s)
{
    if (s.isEmpty()) {
        return;
    }
    int cnt = s.count();
    int j = 0;
    QList<QAction *> list = d->m_customizeGroup->actions();
    for (int i = 0, count = list.count(); i < count; ++i) {
        if (QuickAccessAction *act = dynamic_cast<QuickAccessAction*>(list[i])) {
            if (j < cnt) {
                if (s.at(j) == '1') {
                    d->setActionVisible(act, act->m_srcAction, true);
                } else {
                    if (act->isChecked())
                        d->setActionVisible(act, act->m_srcAction, false);
                }
            }
            ++j;
        }
    }
}

void QuickAccessBar::customizeAction(QAction *action)
{
    d->m_customizeAction = true;
    if (QuickAccessAction *act = dynamic_cast<QuickAccessAction*>(action)) {
        setActionVisible(act->m_srcAction, !widgetForAction(act->m_srcAction));
        emit customizeActionChanged();
    }
    d->m_customizeAction = false;
}

void QuickAccessBar::aboutToShowCustomizeMenu()
{
    d->m_menu->clear();
    d->m_menu->setSeparatorsCollapsible(false);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    d->m_menu->addSection(tr("Customize Quick Access Toolbar"));
#else
    d->m_menu->addSeparator()->setText(tr("Customize Quick Access Toolbar"));
#endif
    foreach (QAction *action, d->m_actionList) {
        d->m_menu->addAction(action);
    }
    emit showCustomizeMenu(d->m_menu);
}

void QuickAccessBar::aboutToHideCustomizeMenu()
{
    d->m_menu->clear();
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
    QToolBar::actionEvent(event);
    if (d->m_actionAccessPopup)  {
        if (event->type() == QEvent::ActionAdded) {
            if (!d->m_removeAction) {
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
                d->m_removeAction = true;
                addAction(d->m_actionAccessPopup);
                d->m_removeAction = false;
            } else {
                d->updateAction(event->action());
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

QCANPOOL_END_NAMESPACE
