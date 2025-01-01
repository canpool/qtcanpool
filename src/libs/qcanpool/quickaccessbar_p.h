/**
 * Copyright (C) 2021-2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#ifndef QUICKACCESSBAR_P_H
#define QUICKACCESSBAR_P_H

//
// W A R N I N G
// -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qcanpool.h"
#include <QToolBar>
#include <QToolButton>
#include <QAction>

class QActionGroup;

QCANPOOL_BEGIN_NAMESPACE

class QuickAccessBar;

/* QuickAccessButton */
class QuickAccessButton : public QToolButton
{
    Q_OBJECT
public:
    explicit QuickAccessButton(QWidget* parent = Q_NULLPTR);
    virtual ~QuickAccessButton();

public:
    virtual QSize sizeHint() const;

public Q_SLOTS:
    void setOrientation(Qt::Orientation orientation);
private:
    Qt::Orientation m_orientation;
};

/* QuickAccessAction */
class QuickAccessAction : public QAction
{
public:
    QuickAccessAction(QObject *p, QAction *srcAction)
        : QAction(srcAction->text(), p)
        , m_srcAction(srcAction)
    {
        setCheckable(true);
        QToolBar *toolBar = qobject_cast<QToolBar *>(parent());
        Q_ASSERT(toolBar);
        setChecked(toolBar->widgetForAction(srcAction));
    }
    void update()
    {
        QToolBar *toolBar = qobject_cast<QToolBar *>(parent());
        Q_ASSERT(toolBar);
        setChecked(toolBar->widgetForAction(m_srcAction));
        setText(m_srcAction->text());
    }
public:
    QAction *m_srcAction;
};

/* QuickAccessBarPrivate */
class QuickAccessBarPrivate : public QObject
{
    Q_OBJECT
public:
    explicit QuickAccessBarPrivate();
    virtual ~QuickAccessBarPrivate();

public:
    void init();
    QuickAccessAction *findQuickAccessAction(QAction* action) const;
    QAction *findBeforeAction(QAction *action) const;
    void updateAction(QAction *action);
    void setActionVisible(QAction *action, bool visible);
    void setActionVisible(QuickAccessAction *wrapper, QAction *action, bool visible);
private Q_SLOTS:
    void customizeAction(QAction *action);
    void aboutToShowCustomizeMenu();
    void aboutToHideCustomizeMenu();
public:
    QuickAccessBar *q;
    QMenu *m_menu;
    QAction *m_actionAccessPopup;
    QActionGroup *m_customizeGroup;
    QuickAccessButton *m_accessPopup;
    QList<QAction *> m_actionList;
    bool m_removingAction : 1;
    bool m_customizingAction : 1;
};

QCANPOOL_END_NAMESPACE

#endif // QUICKACCESSBAR_P_H
