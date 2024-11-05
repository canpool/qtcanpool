/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include <QWidget>

QX_DOCK_BEGIN_NAMESPACE

class DockTabBarPrivate;

class QX_DOCK_EXPORT DockTabBar : public QWidget
{
    Q_OBJECT
public:
    explicit DockTabBar(QWidget *parent = nullptr);
    virtual ~DockTabBar();

private:
    QX_DECLARE_PRIVATE(DockTabBar)
};

QX_DOCK_END_NAMESPACE
