/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef TINYNAVBAR_H
#define TINYNAVBAR_H

#include "qcanpool.h"
#include "qxapp/qxnavbar.h"

QCANPOOL_BEGIN_NAMESPACE

/*!
 * Compatibility shim for 3.0 code (decision K5): the widget is now
 * QxApp::QxNavBar. See tinytabbar.h for why the shims for the tab family are
 * distinct types rather than aliases.
 */
class QCANPOOL_SHARED_EXPORT QCANPOOL_DEPRECATED_X("use QxApp::QxNavBar instead") TinyNavBar :
    public QX_APP_PREPEND_NAMESPACE(QxNavBar)
{
    Q_OBJECT
public:
    explicit TinyNavBar(QWidget *parent = Q_NULLPTR)
        : QX_APP_PREPEND_NAMESPACE(QxNavBar)(parent)
    {
    }
};

QCANPOOL_END_NAMESPACE

#endif   // TINYNAVBAR_H
