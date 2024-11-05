/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockmanager.h"
#include "dockutils.h"

QX_DOCK_BEGIN_NAMESPACE

static DockManager::ConfigFlags s_configFlags = DockManager::DefaultNonOpaqueConfig;

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
    QX_FINI_PRIVATE()
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

DockIconProvider &DockManager::iconProvider()
{
    static DockIconProvider instance;
    return instance;
}

QX_DOCK_END_NAMESPACE