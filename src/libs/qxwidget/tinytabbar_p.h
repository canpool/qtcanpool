/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxwidget_global.h"
#include <QObject>
#include <QMap>

class QAction;
class QActionGroup;
class QToolButton;

QX_WIDGET_BEGIN_NAMESPACE

class TinyTabBar;

class TinyTabBarPrivate : public QObject
{
    Q_OBJECT
public:
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
    QList<QAction *> m_tabs;
    QActionGroup *m_group;
    int m_currentIndex;
    bool m_togglable;
};

QX_WIDGET_END_NAMESPACE
