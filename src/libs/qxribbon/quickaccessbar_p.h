/**
 * Copyright (C) 2021-2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

//
// W A R N I N G
// -------------
//
// This file is not part of the QxWidget API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qxribbon_global.h"
#include <QObject>
#include <QToolBar>
#include <QToolButton>
#include <QAction>

QX_RIBBON_BEGIN_NAMESPACE

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

public slots:
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
        setChecked(m_srcAction->isVisible());
    }
    void update()
    {
        setChecked(m_srcAction->isVisible());
        setText(m_srcAction->text());
    }
public:
    QAction *m_srcAction;
};

/* QuickAccessBarPrivate */
class QuickAccessBarPrivate : public QObject
{
    Q_OBJECT
    QX_DECLARE_PUBLIC(QuickAccessBar)
public:
    explicit QuickAccessBarPrivate();
    virtual ~QuickAccessBarPrivate();
public:
    void init();
    QuickAccessAction *findQuickAccessAction(QAction* action) const;
    void updateAction(QAction *action);
    void setActionVisible(QAction *action, bool visible);
    void setActionVisible(QuickAccessAction *wrapper, QAction *action, bool visible);
private Q_SLOTS:
    void customizeAction(QAction *action);
    void aboutToShowCustomizeMenu();
    void aboutToHideCustomizeMenu();
public:
    QMenu *m_menu;
    QAction *m_actionAccessPopup;
    QActionGroup *m_customizeGroup;
    QuickAccessButton *m_accessPopup;
    QList<QAction *> m_actionList;
    bool m_removingAction : 1;
};

QX_RIBBON_END_NAMESPACE
