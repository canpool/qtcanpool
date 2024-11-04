/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock_global.h"
#include <QWidget>

QX_DOCK_BEGIN_NAMESPACE

class DockWidgetPrivate;

class QX_DOCK_EXPORT DockWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DockWidget(QWidget *parent = nullptr);
    ~DockWidget();

private:
    QX_DECLARE_PRIVATE(DockWidget)
};

QX_DOCK_END_NAMESPACE
