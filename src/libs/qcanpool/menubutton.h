/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MENUBUTTON_H
#define MENUBUTTON_H

#include "qcanpool.h"
#include "qxapp/qxmenubutton.h"

QCANPOOL_BEGIN_NAMESPACE

/*!
 * Compatibility shim for 3.0 code (decision K5): the widget is now
 * QxApp::QxMenuButton. See fancytoolbutton.h for the rationale; the name
 * disappears again in 3.2.
 */
class QCANPOOL_SHARED_EXPORT QCANPOOL_DEPRECATED_X("use QxApp::QxMenuButton instead") MenuButton :
    public QX_APP_PREPEND_NAMESPACE(QxMenuButton)
{
    Q_OBJECT
public:
    explicit MenuButton(QWidget *parent = Q_NULLPTR)
        : QX_APP_PREPEND_NAMESPACE(QxMenuButton)(parent)
    {
    }
    explicit MenuButton(const QString &text, QWidget *parent = Q_NULLPTR)
        : QX_APP_PREPEND_NAMESPACE(QxMenuButton)(text, parent)
    {
    }
    MenuButton(const QIcon &icon, const QString &text, QWidget *parent = Q_NULLPTR)
        : QX_APP_PREPEND_NAMESPACE(QxMenuButton)(icon, text, parent)
    {
    }
};

QCANPOOL_END_NAMESPACE

#endif   // MENUBUTTON_H
