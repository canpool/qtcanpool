/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxapp_global.h"
#include "qxtabbar_p.h"
#include "qxnavbar.h"   // must, for Q_DECLARE_PUBLIC -> static_cast
#include <QToolButton>
#include <QMap>

class QMenu;

QX_APP_BEGIN_NAMESPACE

class QxMenuAccessButton;
class QxNavBar;

class QxNavBarPrivate : public QxTabBarPrivate
{
    Q_OBJECT
public:
    Q_DECLARE_PUBLIC(QxNavBar)
public:
    QxNavBarPrivate();
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
    QxMenuAccessButton *m_accessPopup;
    QList<QAction *> m_actionList;
    QMap<QAction *, QAction *> m_actionMap;   // lowAction,checkAction
    bool m_removingAction : 1;
};

QX_APP_END_NAMESPACE
