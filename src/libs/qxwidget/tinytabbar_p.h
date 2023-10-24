/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxglobal.h"
#include <QObject>
#include <QMap>

class QAction;
class QActionGroup;
class QToolButton;

QX_BEGIN_NAMESPACE

class TinyTabBar;

class TinyTabBarPrivate : public QObject
{
    Q_OBJECT
    QX_DECLARE_PUBLIC(TinyTabBar)
public:
    TinyTabBarPrivate();
    virtual ~TinyTabBarPrivate();

    void init();
    bool validIndex(int index) const
    { return index >= 0 && index < m_tabs.count(); }

    int indexOf(QAction *action);

private Q_SLOTS:
    void onTriggered(QAction *action);
public:
    QList<QToolButton *> m_tabs;
    QMap<QToolButton *, QAction *> m_buttonActionMap;
    QActionGroup *m_group;
    int m_currentIndex;
    bool m_togglable;
};

QX_END_NAMESPACE
