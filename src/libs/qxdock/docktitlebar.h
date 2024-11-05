/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include <QWidget>

QX_DOCK_BEGIN_NAMESPACE

class DockTitleBarPrivate;

class QX_DOCK_EXPORT DockTitleBar : public QWidget
{
    Q_OBJECT
public:
    explicit DockTitleBar(QWidget *parent = nullptr);
    virtual ~DockTitleBar();

private:
    QX_DECLARE_PRIVATE(DockTitleBar)
};

QX_DOCK_END_NAMESPACE
