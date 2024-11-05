/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include <QIcon>

QX_DOCK_BEGIN_NAMESPACE

class DockIconProviderPrivate;

class QX_DOCK_EXPORT DockIconProvider
{
public:
    DockIconProvider();
    ~DockIconProvider();

    QIcon customIcon(Qx::DockIcon id) const;
    void registerCustomIcon(Qx::DockIcon id, const QIcon &icon);

private:
    QX_DECLARE_PRIVATE(DockIconProvider)
};

QX_DOCK_END_NAMESPACE
