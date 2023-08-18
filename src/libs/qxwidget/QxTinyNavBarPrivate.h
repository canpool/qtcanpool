/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "QxGlobal.h"
#include "QxTinyTabBarPrivate.h"
#include <QMap>

class QMenu;

QX_BEGIN_NAMESPACE

class QuickAccessButton;
class TinyNavBar;

class TinyNavBarPrivate : public TinyTabBarPrivate
{
    Q_OBJECT
    QX_DECLARE_PUBLIC(TinyNavBar)
public:
    TinyNavBarPrivate();
public:
    void init();
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
    QMap<QAction *, QAction *> m_actionMap; // lowAction,checkAction
    bool m_removingAction : 1;
};

QX_END_NAMESPACE
