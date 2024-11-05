/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockwindow.h"
#include "dockpanel.h"

QX_DOCK_BEGIN_NAMESPACE

class DockWindowPrivate
{
public:
    QX_DECLARE_PUBLIC(DockWindow)
public:
    DockWindowPrivate();
public:
    QList<DockContainer *> m_containers;
};

DockWindowPrivate::DockWindowPrivate()
{
}

DockWindow::DockWindow(QWidget *parent)
    : DockContainer{this, parent}
{
    QX_INIT_PRIVATE(DockWindow)
}

DockWindow::~DockWindow()
{
    QX_FINI_PRIVATE()
}

DockPanel *DockWindow::addDockWidget(Qx::DockWidgetArea area, DockWidget *w, DockPanel *p, int index)
{
    DockContainer *container = p ? p->dockContainer() : this;
    if (container == nullptr) {
        container = this;
    }
    DockPanel *panel = container->addDockWidget(area, w, p, index);
    if (panel) {
        Q_EMIT dockWidgetAdded(w);
    }
    return panel;
}

void DockWindow::registerDockContainer(DockContainer *container)
{
    Q_D(DockWindow);
    d->m_containers.append(container);
}

QX_DOCK_END_NAMESPACE
