/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef TINYTABWIDGET_H
#define TINYTABWIDGET_H

#include "qcanpool.h"
#include "qxapp/qxtabwidget.h"

QCANPOOL_BEGIN_NAMESPACE

/*!
 * Compatibility shim for 3.0 code (decision K5): the widget is now
 * QxApp::QxTabWidget. See tinytabbar.h for why the shims for the tab family
 * are distinct types rather than aliases; note in particular that
 * tabBar() now returns QxApp::QxTabBar*.
 */
class QCANPOOL_SHARED_EXPORT QCANPOOL_DEPRECATED_X("use QxApp::QxTabWidget instead") TinyTabWidget :
    public QX_APP_PREPEND_NAMESPACE(QxTabWidget)
{
    Q_OBJECT
public:
    explicit TinyTabWidget(QWidget *parent = Q_NULLPTR)
        : QX_APP_PREPEND_NAMESPACE(QxTabWidget)(parent)
    {
    }
};

QCANPOOL_END_NAMESPACE

#endif   // TINYTABWIDGET_H
