/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MENUACCESSBUTTON_H
#define MENUACCESSBUTTON_H

#include "qcanpool.h"
#include "qxapp/qxmenuaccessbutton.h"

QCANPOOL_BEGIN_NAMESPACE

/*!
 * Compatibility shim for 3.0 code (decision K5): the widget is now
 * QxApp::QxMenuAccessButton. See fancytoolbutton.h for the rationale; the name
 * disappears again in 3.2.
 */
class QCANPOOL_SHARED_EXPORT QCANPOOL_DEPRECATED_X("use QxApp::QxMenuAccessButton instead") MenuAccessButton :
    public QX_APP_PREPEND_NAMESPACE(QxMenuAccessButton)
{
    Q_OBJECT
public:
    explicit MenuAccessButton(QWidget *parent = Q_NULLPTR)
        : QX_APP_PREPEND_NAMESPACE(QxMenuAccessButton)(parent)
    {
    }
};

QCANPOOL_END_NAMESPACE

#endif   // MENUACCESSBUTTON_H
