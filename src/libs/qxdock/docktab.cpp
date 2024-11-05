/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "docktab.h"
#include "dockwidget.h"
#include "dockpanel.h"

QX_DOCK_BEGIN_NAMESPACE

class DockTabPrivate
{
public:
    QX_DECLARE_PUBLIC(DockTab)
public:
    DockTabPrivate();
    void init();
public:
    DockWidget *m_dockWidget = nullptr;
    DockPanel *m_panel = nullptr;
};

DockTabPrivate::DockTabPrivate()
{

}

void DockTabPrivate::init()
{

}

DockTab::DockTab(DockWidget *w, QWidget *parent)
    : QWidget(parent)
{
    QX_INIT_PRIVATE(DockTab);

    Q_D(DockTab);
    d->m_dockWidget = w;
    d->init();

    setAttribute(Qt::WA_NoMousePropagation, true);
    setFocusPolicy(Qt::NoFocus);
}

DockTab::~DockTab()
{
    QX_FINI_PRIVATE()
}

void DockTab::setDockPanel(DockPanel *panel)
{
    Q_D(DockTab);
    d->m_panel = panel;
}

QX_DOCK_END_NAMESPACE
