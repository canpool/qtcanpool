/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include "dockiconprovider.h"

QX_DOCK_BEGIN_NAMESPACE

class DockManagerPrivate;

class QX_DOCK_EXPORT DockManager
{
public:
    enum ConfigFlag {
        DockAreaHasCloseButton = 0x0002,
        DockAreaHasUndockButton = 0x4000,
        DockAreaHasTabsMenuButton = 0x8000,

        DefaultDockAreaButtons = DockAreaHasCloseButton
                               | DockAreaHasUndockButton
                               | DockAreaHasTabsMenuButton,

        DefaultBaseConfig = DefaultDockAreaButtons,

        DefaultNonOpaqueConfig = DefaultBaseConfig
    };
    Q_DECLARE_FLAGS(ConfigFlags, ConfigFlag)

public:
    DockManager();
    ~DockManager();

    static ConfigFlags configFlags();
    static void setConfigFlags(const ConfigFlags flags);
    static void setConfigFlag(ConfigFlag flag, bool on = true);
    static bool testConfigFlag(ConfigFlag flag);

    static DockIconProvider &iconProvider();

private:
    QX_DECLARE_PRIVATE(DockManager)
};

QX_DOCK_END_NAMESPACE
