/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxapp_global.h"
#include "qxtabbar.h"

QX_APP_BEGIN_NAMESPACE

class QxNavBarPrivate;

class QX_APP_EXPORT QxNavBar : public QxTabBar
{
    Q_OBJECT
public:
    explicit QxNavBar(QWidget *parent = Q_NULLPTR);
    virtual ~QxNavBar();
public:
    QAction *actionCustomizeButton() const;
    int visibleCount() const;
Q_SIGNALS:
    void showCustomizeMenu(QMenu *menu);
    void customizeTabChanged();
protected:
    virtual void actionEvent(QActionEvent *event);
private:
    Q_DECLARE_PRIVATE(QxNavBar)
    Q_DISABLE_COPY(QxNavBar)
};

QX_APP_END_NAMESPACE
