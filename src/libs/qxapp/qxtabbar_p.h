/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxapp_global.h"
#include <QObject>
#include <QMap>

class QAction;
class QActionGroup;
class QToolButton;

QX_APP_BEGIN_NAMESPACE

class QxTabBar;

class QxTabBarPrivate : public QObject
{
    Q_OBJECT
public:
    QX_DECLARE_PUBLIC(QxTabBar)
public:
    QxTabBarPrivate();
    virtual ~QxTabBarPrivate();

    void init();
    bool validIndex(int index) const
    {
        return index >= 0 && index < m_tabs.count();
    }

    int indexOf(QAction *action);

    void layoutActions();

private Q_SLOTS:
    void onTriggered(QAction *action);
    void onOrientationChanged(Qt::Orientation orientation);
public:
    QList<QAction *> m_tabs;
    QActionGroup *m_group;
    int m_currentIndex;
    bool m_togglable;
};

QX_APP_END_NAMESPACE
