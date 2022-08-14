/**
 * Copyright (C) 2021-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#ifndef QUICKACCESSBAR_H
#define QUICKACCESSBAR_H

#include "qcanpool_global.h"
#include <QToolBar>
#include <QMenu>

QCANPOOL_BEGIN_NAMESPACE

class QuickAccessBarPrivate;

/* QuickAccessBar */
class QCANPOOL_SHARED_EXPORT QuickAccessBar : public QToolBar
{
    Q_OBJECT
public:
    explicit QuickAccessBar(QWidget *parent = Q_NULLPTR);
    virtual ~QuickAccessBar();

public:
    QAction *actionCustomizeButton() const;
    void setActionVisible(QAction *action, bool visible);
    bool isActionVisible(QAction *action) const;
    int visibleCount() const;

Q_SIGNALS:
    void showCustomizeMenu(QMenu *menu);

private Q_SLOTS:
    void customizeAction(QAction *action);
    void aboutToShowCustomizeMenu();
    void aboutToHideCustomizeMenu();

protected:
    virtual bool event(QEvent *event);
    virtual void actionEvent(QActionEvent *event);

private:
    QuickAccessBarPrivate *d;
};

QCANPOOL_END_NAMESPACE

#endif // QUICKACCESSBAR_H
