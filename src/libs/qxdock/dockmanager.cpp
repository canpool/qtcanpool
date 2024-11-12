/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockmanager.h"
#include "dockutils.h"

#include <QApplication>

QX_DOCK_BEGIN_NAMESPACE

static DockManager::ConfigFlags s_configFlags = DockManager::DefaultNonOpaqueConfig;
static DockManager::AutoHideFlags s_autoHideConfigFlags; // auto hide feature is disabled by default

class DockManagerPrivate
{
public:
    QX_DECLARE_PUBLIC(DockManager)
public:
    DockManagerPrivate();
};

DockManagerPrivate::DockManagerPrivate()
{
}

DockManager::DockManager()
{
    QX_INIT_PRIVATE(DockManager);
}

DockManager::~DockManager()
{
    QX_FINI_PRIVATE();
}

DockManager::ConfigFlags DockManager::configFlags()
{
    return s_configFlags;
}

void DockManager::setConfigFlags(const ConfigFlags flags)
{
    s_configFlags = flags;
}

void DockManager::setConfigFlag(DockManager::ConfigFlag flag, bool on)
{
    internal::setFlag(s_configFlags, flag, on);
}

bool DockManager::testConfigFlag(DockManager::ConfigFlag flag)
{
    return configFlags().testFlag(flag);
}

DockManager::AutoHideFlags DockManager::autoHideConfigFlags()
{
    return s_autoHideConfigFlags;
}

void DockManager::setAutoHideConfigFlags(const AutoHideFlags flags)
{
    s_autoHideConfigFlags = flags;
}

void DockManager::setAutoHideConfigFlag(DockManager::AutoHideFlag flag, bool on)
{
    internal::setFlag(s_autoHideConfigFlags, flag, on);
}

bool DockManager::testAutoHideConfigFlag(DockManager::AutoHideFlag flag)
{
    return autoHideConfigFlags().testFlag(flag);
}

DockIconProvider &DockManager::iconProvider()
{
    static DockIconProvider instance;
    return instance;
}

int DockManager::startDragDistance()
{
    return QApplication::startDragDistance() * 1.5;
}


QX_DOCK_END_NAMESPACE
