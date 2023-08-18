/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "QxGlobal.h"
#include "QxTinyTabBar.h"

QX_BEGIN_NAMESPACE

class TinyNavBarPrivate;

class QX_WIDGET_EXPORT TinyNavBar : public TinyTabBar
{
    Q_OBJECT
public:
    explicit TinyNavBar(QWidget *parent = Q_NULLPTR);
    virtual ~TinyNavBar();
public:
    QAction *actionCustomizeButton() const;
    int visibleCount() const;
Q_SIGNALS:
    void showCustomizeMenu(QMenu *menu);
    void customizeTabChanged();
protected:
    virtual void actionEvent(QActionEvent *event);
private:
    Q_DECLARE_PRIVATE(TinyNavBar)
    Q_DISABLE_COPY(TinyNavBar)
};

QX_END_NAMESPACE
