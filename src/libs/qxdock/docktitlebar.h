/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include <QWidget>

class QAbstractButton;

QX_DOCK_BEGIN_NAMESPACE

class DockPanel;

class DockTitleBarPrivate;

class QX_DOCK_EXPORT DockTitleBar : public QWidget
{
    Q_OBJECT
public:
    explicit DockTitleBar(DockPanel *parent = nullptr);
    virtual ~DockTitleBar();

    QAbstractButton *button(Qx::DockTitleBarButton which) const;

    QString titleBarButtonToolTip(Qx::DockTitleBarButton id) const;

private:
    QX_DECLARE_PRIVATE(DockTitleBar)
};

QX_DOCK_END_NAMESPACE