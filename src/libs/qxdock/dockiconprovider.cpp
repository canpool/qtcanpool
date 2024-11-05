/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockiconprovider.h"

#include <QVector>

QX_DOCK_BEGIN_NAMESPACE

class DockIconProviderPrivate
{
public:
    QX_DECLARE_PUBLIC(DockIconProvider)
public:
    DockIconProviderPrivate();
public:
    QVector<QIcon> m_userIcons{Qx::NDockIconSizes, QIcon()};
};

DockIconProviderPrivate::DockIconProviderPrivate()
{

}

DockIconProvider::DockIconProvider()
{
    QX_INIT_PRIVATE(DockIconProvider);
}

DockIconProvider::~DockIconProvider()
{
    QX_FINI_PRIVATE()
}

QIcon DockIconProvider::customIcon(Qx::DockIcon id) const
{
    Q_D(const DockIconProvider);
    Q_ASSERT(id < d->m_userIcons.size());
    return d->m_userIcons[id];
}

void DockIconProvider::registerCustomIcon(Qx::DockIcon id, const QIcon &icon)
{
    Q_D(DockIconProvider);
    Q_ASSERT(id < d->m_userIcons.size());
    d->m_userIcons[id] = icon;
}

QX_DOCK_END_NAMESPACE
