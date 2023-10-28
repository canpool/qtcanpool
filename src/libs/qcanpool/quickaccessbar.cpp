/**
 * Copyright (C) 2021-2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#include "quickaccessbar_p.h"
#include "quickaccessbar.h"

#include <QToolButton>
#include <QAction>
#include <QStyle>
#include <QEvent>
#include <QActionEvent>
#include <QActionGroup>
#include <QLayout>

QCANPOOL_BEGIN_NAMESPACE

QuickAccessButton::QuickAccessButton(QWidget *parent)
    : QToolButton(parent)
    , m_orientation(Qt::Horizontal)
{
    setObjectName(QLatin1String("qtc_quickaccessbutton"));
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


QuickAccessBarPrivate::QuickAccessBarPrivate()
    : m_menu(Q_NULLPTR)
    , m_actionAccessPopup(Q_NULLPTR)
    , m_customizeGroup(Q_NULLPTR)
    , m_accessPopup(Q_NULLPTR)
    , m_removingAction(false)
    , m_customizingAction(false)
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
        if (m_customizingAction) {
            QAction *beforeAct = findBeforeAction(wrapper);
            q->insertAction(beforeAct, action);
        } else {
            q->addAction(action);
        }
    } else {
        q->removeAction(action);
        m_removingAction = false;
    }
    wrapper->update();
    q->adjustSize();
}

void QuickAccessBarPrivate::customizeAction(QAction *action)
{
    m_customizingAction = true;
    if (QuickAccessAction *act = dynamic_cast<QuickAccessAction*>(action)) {
        setActionVisible(act, act->m_srcAction, !q->widgetForAction(act->m_srcAction));
        emit q->customizeActionChanged();
    }
    m_customizingAction = false;
}

void QuickAccessBarPrivate::aboutToShowCustomizeMenu()
{
    m_menu->clear();
    m_menu->setSeparatorsCollapsible(false);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    m_menu->addSection(tr("Customize Quick Access Toolbar"));
#else
    m_menu->addSeparator()->setText(tr("Customize Quick Access Toolbar"));
#endif
    foreach (QAction *action, m_actionList) {
        m_menu->addAction(action);
    }
    emit q->showCustomizeMenu(m_menu);
}

void QuickAccessBarPrivate::aboutToHideCustomizeMenu()
{
    m_menu->clear();
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
        s.append(act->isChecked() ? '1' : '0');
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
