/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef TINYTABBAR_H
#define TINYTABBAR_H

#include "qcanpool.h"
#include "qxapp/qxtabbar.h"

QCANPOOL_BEGIN_NAMESPACE

class TinyTabBarPrivate;

/*!
 * Compatibility shim for 3.0 code (decision K5): the widget is now
 * QxApp::QxTabBar. See fancytoolbutton.h for the rationale.
 *
 * Unlike the button shims this one is not a transparent alias: TinyTabBar is a
 * *distinct* type now, so code that mixed the old types (e.g. keeping a
 * TinyTabBar pointer to a TinyNavBar, or taking the result of
 * TinyTabWidget::tabBar()) has to be updated to QxTabBar. That is deliberate -
 * a distinct deprecated name is the migration point, and the alternative
 * (aliasing the name) would silence the warning that points at it.
 */
class QCANPOOL_SHARED_EXPORT QCANPOOL_DEPRECATED_X("use QxApp::QxTabBar instead") TinyTabBar :
    public QX_APP_PREPEND_NAMESPACE(QxTabBar)
{
    Q_OBJECT
public:
    explicit TinyTabBar(QWidget *parent = Q_NULLPTR)
        : QX_APP_PREPEND_NAMESPACE(QxTabBar)(parent)
    {
    }
protected:
    /*! Retained for subclasses that build the private data themselves. */
    explicit TinyTabBar(QX_APP_PREPEND_NAMESPACE(QxTabBarPrivate) * d, QWidget *parent = Q_NULLPTR)
        : QX_APP_PREPEND_NAMESPACE(QxTabBar)(d, parent)
    {
    }
};

QCANPOOL_END_NAMESPACE

#endif   // TINYTABBAR_H
