/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include <QObject>

QX_DOCK_BEGIN_NAMESPACE

class DockFocusControllerPrivate;

class QX_DOCK_EXPORT DockFocusController : public QObject
{
    Q_OBJECT
public:
    explicit DockFocusController(QObject *parent = nullptr);
    virtual ~DockFocusController();

private:
    QX_DECLARE_PRIVATE(DockFocusController)
};

QX_DOCK_END_NAMESPACE
