/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "ribbonmenu.h"

#include <QWidgetAction>

QX_RIBBON_BEGIN_NAMESPACE

/* RibbonMenu */
RibbonMenu::RibbonMenu(QWidget *parent)
    : RibbonMenu(QString(), parent)
{
}

RibbonMenu::RibbonMenu(const QString &title, QWidget *parent)
    : QMenu(title, parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMouseTracking(true);
}

QAction *RibbonMenu::addRibbonMenu(RibbonMenu *menu)
{
    return QMenu::addMenu(menu);
}

RibbonMenu *RibbonMenu::addRibbonMenu(const QString &title)
{
    RibbonMenu *menu = new RibbonMenu(title, this);
    addAction(menu->menuAction());
    return menu;
}

RibbonMenu *RibbonMenu::addRibbonMenu(const QIcon &icon, const QString &title)
{
    RibbonMenu *menu = new RibbonMenu(title, this);
    menu->setIcon(icon);
    addAction(menu->menuAction());
    return menu;
}

QAction *RibbonMenu::addWidget(QWidget *w)
{
    QWidgetAction *action = new QWidgetAction(this);
    action->setDefaultWidget(w);
    addAction(action);
    return action;
}

QX_RIBBON_END_NAMESPACE
