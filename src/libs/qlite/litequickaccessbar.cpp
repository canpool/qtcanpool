/***************************************************************************
 **
 **  Copyright (C) 2021-2022 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **           https://gitee.com/icanpool
 **
 **  Powered by QtitanRibbon
 **
 **  GNU Lesser General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU Lesser
 **  General Public License version 3 as published by the Free Software
 **  Foundation and appearing in the file LICENSE.LGPL3 included in the
 **  packaging of this file. Please review the following information to
 **  ensure the GNU Lesser General Public License version 3 requirements
 **  will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
 **
 **  GNU General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU
 **  General Public License version 2.0 or (at your option) the GNU General
 **  Public license version 3 or any later version approved by the KDE Free
 **  Qt Foundation. The licenses are as published by the Free Software
 **  Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
 **  included in the packaging of this file. Please review the following
 **  information to ensure the GNU General Public License requirements will
 **  be met: https://www.gnu.org/licenses/gpl-2.0.html and
 **  https://www.gnu.org/licenses/gpl-3.0.html.
 **
****************************************************************************/
#include "litequickaccessbar.h"

#include <QToolButton>
#include <QAction>
#include <QStyle>
#include <QEvent>
#include <QActionEvent>

QLITE_BEGIN_NAMESPACE

/* LiteQuickAccessButton */
class LiteQuickAccessButton : public QToolButton
{
public:
    explicit LiteQuickAccessButton(QWidget* parent = Q_NULLPTR);
    virtual ~LiteQuickAccessButton();

public:
    virtual QSize sizeHint() const;
};

LiteQuickAccessButton::LiteQuickAccessButton(QWidget *parent)
    : QToolButton(parent)
{
    setObjectName(QLatin1String("qlite_quickaccessbutton"));
}

LiteQuickAccessButton::~LiteQuickAccessButton()
{

}

QSize LiteQuickAccessButton::sizeHint() const
{
    QSize sz = QToolButton::sizeHint();
    return QSize(sz.width() / 2, sz.height());
}

/* LiteQuickAccessAction */
class LiteQuickAccessAction : public QAction
{
public:
    LiteQuickAccessAction(QObject *p, QAction *srcAction)
        : QAction(srcAction->text(), p)
        , m_srcAction(srcAction)
    {
        setCheckable(true);
        QToolBar *toolBar = qobject_cast<QToolBar*>(parent());
        Q_ASSERT(toolBar);
        setChecked(toolBar->widgetForAction(srcAction));
    }
    void update()
    {
        QToolBar *toolBar = qobject_cast<QToolBar*>(parent());
        Q_ASSERT(toolBar);
        setChecked(toolBar->widgetForAction(m_srcAction));
        setText(m_srcAction->text());
    }
public:
    QAction *m_srcAction;
};

/* LiteQuickAccessBarPrivate */
class LiteQuickAccessBarPrivate
{
public:
    explicit LiteQuickAccessBarPrivate();
    virtual ~LiteQuickAccessBarPrivate();

public:
    void init();
    LiteQuickAccessAction *findQuickAccessAction(QAction* action) const;
    QAction *findBeforeAction(QAction *action) const;
    void updateAction(QAction *action);
    void setActionVisible(QAction *action, bool visible);

public:
    LiteQuickAccessBar *q;
    QMenu *m_menu;
    QAction *m_actionAccessPopup;
    QActionGroup *m_customizeGroup;
    LiteQuickAccessButton *m_accessPopup;
    QList<QAction*> m_actionList;
    bool m_removeAction : 1;
    bool m_customizeAction : 1;
};

LiteQuickAccessBarPrivate::LiteQuickAccessBarPrivate()
    : m_menu(Q_NULLPTR)
    , m_actionAccessPopup(Q_NULLPTR)
    , m_customizeGroup(Q_NULLPTR)
    , m_accessPopup(Q_NULLPTR)
    , m_removeAction(false)
    , m_customizeAction(false)
{

}

LiteQuickAccessBarPrivate::~LiteQuickAccessBarPrivate()
{

}

void LiteQuickAccessBarPrivate::init()
{
    q->setAutoFillBackground(false);
    q->setAttribute(Qt::WA_NoSystemBackground);
    q->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    const int sz = q->style()->pixelMetric(QStyle::PM_SmallIconSize, Q_NULLPTR, q);
    q->setIconSize(QSize(sz, sz));

    m_accessPopup = new LiteQuickAccessButton(q);
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

LiteQuickAccessAction *LiteQuickAccessBarPrivate::findQuickAccessAction(QAction *action) const
{
    if (m_customizeGroup == Q_NULLPTR)
        return Q_NULLPTR;

    QList<QAction *> list = m_customizeGroup->actions();
    for (int i = 0; i < list.count(); ++i) {
        LiteQuickAccessAction *act = dynamic_cast<LiteQuickAccessAction*>(list[i]);
        if (act && action == act->m_srcAction) {
            return act;
        }
    }
    return Q_NULLPTR;
}

QAction *LiteQuickAccessBarPrivate::findBeforeAction(QAction *action) const
{
    QList<QAction *> list = m_customizeGroup->actions();
    bool find = false;
    for (int i = 0, count = list.count(); i < count; ++i) {
        if (find) {
            if (LiteQuickAccessAction *beforeAct = dynamic_cast<LiteQuickAccessAction*>(list[i])) {
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

void LiteQuickAccessBarPrivate::updateAction(QAction *action)
{
    if (LiteQuickAccessAction* wrapper = findQuickAccessAction(action)) {
        wrapper->update();
    }
}

void LiteQuickAccessBarPrivate::setActionVisible(QAction *action, bool visible)
{
    if (LiteQuickAccessAction *wrapper = findQuickAccessAction(action)) {
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
}

LiteQuickAccessBar::LiteQuickAccessBar(QWidget *parent)
    : QToolBar(parent)
    , d(new LiteQuickAccessBarPrivate())
{
    d->q = this;
    d->init();
    setObjectName(QLatin1String("qlite_quickaccessbar"));
}

LiteQuickAccessBar::~LiteQuickAccessBar()
{
    delete d;
}

QAction *LiteQuickAccessBar::actionCustomizeButton() const
{
    return d->m_actionAccessPopup;
}

void LiteQuickAccessBar::setActionVisible(QAction *action, bool visible)
{
    d->setActionVisible(action, visible);
}

bool LiteQuickAccessBar::isActionVisible(QAction *action) const
{
    if (LiteQuickAccessAction *wrapper = d->findQuickAccessAction(action)) {
        return wrapper->isChecked();
    }
    return false;
}

int LiteQuickAccessBar::visibleCount() const
{
    int visibleCount = 0;
    QList<QAction *> list = d->m_customizeGroup->actions();
    for (int i = 0, count = list.count(); i < count; ++i) {
        if (LiteQuickAccessAction *act = dynamic_cast<LiteQuickAccessAction*>(list[i])) {
            if (act->isChecked()) {
                visibleCount++;
            }
        }
    }
    return visibleCount;
}

void LiteQuickAccessBar::customizeAction(QAction *action)
{
    d->m_customizeAction = true;
    if (LiteQuickAccessAction *act = dynamic_cast<LiteQuickAccessAction*>(action)) {
        setActionVisible(act->m_srcAction, !widgetForAction(act->m_srcAction));
    }
    d->m_customizeAction = false;
}

void LiteQuickAccessBar::aboutToShowCustomizeMenu()
{
    d->m_menu->clear();
    d->m_menu->setSeparatorsCollapsible(false);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    d->m_menu->addSection(tr("Customize Quick Access Toolbar"));
#else
    d->m_menu->addSeparator()->setText(tr("Customize Quick Access Toolbar"));
#endif
    QListIterator<QAction*> itAction(d->m_actionList);
    while (itAction.hasNext()) {
        QAction *action = itAction.next();
        d->m_menu->addAction(action);
    }
    emit showCustomizeMenu(d->m_menu);
}

void LiteQuickAccessBar::aboutToHideCustomizeMenu()
{
    d->m_menu->clear();
}

bool LiteQuickAccessBar::event(QEvent *event)
{
    if (parentWidget() && (event->type() == QEvent::Hide || event->type() == QEvent::Show)) {
        adjustSize();
    } else if (event->type() == QEvent::StyleChange) {
        const int sz = style()->pixelMetric(QStyle::PM_SmallIconSize, Q_NULLPTR, this);
        setIconSize(QSize(sz, sz));
    }
    return QToolBar::event(event);
}

void LiteQuickAccessBar::actionEvent(QActionEvent *event)
{
    QToolBar::actionEvent(event);
    if (d->m_actionAccessPopup)  {
        if (event->type() == QEvent::ActionAdded) {
            if (!d->m_removeAction) {
                removeAction(d->m_actionAccessPopup);
            }
            LiteQuickAccessAction *quickAccessAction = d->findQuickAccessAction(event->action());
            if (event->action() != d->m_actionAccessPopup && !quickAccessAction) {
                LiteQuickAccessAction *act = new LiteQuickAccessAction(this, event->action());
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
            } else if (!dynamic_cast<LiteQuickAccessAction *>(event->action())) {
                d->updateAction(event->action());
            }
        }
    }
}

QLITE_END_NAMESPACE
