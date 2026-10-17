/**
 * Copyright (C) 2021-2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"
#include <QToolBar>
#include <QMenu>

QX_RIBBON_BEGIN_NAMESPACE

class RibbonQuickAccessBarPrivate;

/* RibbonQuickAccessBar */
class QX_RIBBON_EXPORT RibbonQuickAccessBar : public QToolBar
{
    Q_OBJECT
public:
    explicit RibbonQuickAccessBar(QWidget *parent = Q_NULLPTR);
    virtual ~RibbonQuickAccessBar();
public:
    QAction *actionCustomizeButton() const;
    /// The trailing "Customize Quick Access Toolbar..." entry of the customize menu;
    /// triggering it emits customizeDialogRequested() (B8/B10).
    QAction *actionCustomizeDialog() const;
    void setActionVisible(QAction *action, bool visible);
    bool isActionVisible(QAction *action) const;
    int visibleCount() const;

    QByteArray state() const;
    void setState(const QByteArray &s);

Q_SIGNALS:
    void showCustomizeMenu(QMenu *menu);
    void customizeActionChanged();
    /// Emitted when the customize menu's trailing "Customize Quick Access Toolbar..."
    /// entry is triggered; the application opens its customization dialog (B8).
    void customizeDialogRequested();
protected:
    virtual bool event(QEvent *event);
    virtual void actionEvent(QActionEvent *event);
private:
    QX_DECLARE_PRIVATE(RibbonQuickAccessBar)
};

QX_RIBBON_END_NAMESPACE
