/**
 * Copyright (C) 2021-2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "QxGlobal.h"
#include <QToolBar>

QX_BEGIN_NAMESPACE

class QuickAccessBarPrivate;

/* QuickAccessBar */
class QX_WIDGET_EXPORT QuickAccessBar : public QToolBar
{
    Q_OBJECT
public:
    explicit QuickAccessBar(QWidget *parent = Q_NULLPTR);
    virtual ~QuickAccessBar();

    QAction *actionCustomizeButton() const;

    void setActionVisible(QAction *action, bool visible);
    bool isActionVisible(QAction *action) const;
    int visibleCount() const;

    QByteArray state() const;
    void setState(const QByteArray &s);

Q_SIGNALS:
    void showCustomizeMenu(QMenu *menu);
    void customizeActionChanged();
protected:
    virtual bool event(QEvent *event);
    virtual void actionEvent(QActionEvent *event);
private:
    QX_DECLARE_PRIVATE(QuickAccessBar)
    Q_DISABLE_COPY(QuickAccessBar)
};

QX_END_NAMESPACE
