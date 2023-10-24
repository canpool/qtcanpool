/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxwidget_global.h"
#include "tinytabbar_p.h"
#include "menuaccessbutton.h"
#include <QMap>

class QMenu;

QX_WIDGET_BEGIN_NAMESPACE

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
    MenuAccessButton *m_accessPopup;
    QList<QAction *> m_actionList;
    QMap<QAction *, QAction *> m_actionMap; // lowAction,checkAction
    bool m_removingAction : 1;
};

QX_WIDGET_END_NAMESPACE
