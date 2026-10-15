/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxapp_global.h"
#include "qxtabbar.h"

QX_APP_BEGIN_NAMESPACE

class QxNavTabBarPrivate;

class QX_APP_EXPORT QxNavTabBar : public QxTabBar
{
    Q_OBJECT
public:
    explicit QxNavTabBar(QWidget *parent = Q_NULLPTR);
    virtual ~QxNavTabBar();
public:
    QAction *actionCustomizeButton() const;
    int visibleCount() const;
Q_SIGNALS:
    void showCustomizeMenu(QMenu *menu);
    void customizeTabChanged();
protected:
    virtual void actionEvent(QActionEvent *event);
private:
    Q_DECLARE_PRIVATE(QxNavTabBar)
    Q_DISABLE_COPY(QxNavTabBar)
};

QX_APP_END_NAMESPACE
