/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock_global.h"
#include <QWidget>

QX_DOCK_BEGIN_NAMESPACE

class DockPanelPrivate;

class QX_DOCK_EXPORT DockPanel : public QWidget
{
    Q_OBJECT
public:
    explicit DockPanel(QWidget *parent = nullptr);
    ~DockPanel();

private:
    QX_DECLARE_PRIVATE(DockPanel)
};

QX_DOCK_END_NAMESPACE
