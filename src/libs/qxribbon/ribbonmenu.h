/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "qxribbon_global.h"
#include <QMenu>

QX_RIBBON_BEGIN_NAMESPACE

/* RibbonMenu */
class QX_RIBBON_EXPORT RibbonMenu : public QMenu
{
    Q_OBJECT
public:
    explicit RibbonMenu(QWidget *parent = Q_NULLPTR);
    explicit RibbonMenu(const QString &title, QWidget *parent = Q_NULLPTR);

    QAction *addRibbonMenu(RibbonMenu *menu);
    RibbonMenu *addRibbonMenu(const QString &title);
    RibbonMenu *addRibbonMenu(const QIcon &icon, const QString &title);
    QAction *addWidget(QWidget *w);
};

QX_RIBBON_END_NAMESPACE
