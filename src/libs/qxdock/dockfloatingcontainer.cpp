/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockfloatingcontainer.h"
#include "dockwindow.h"
#include "dockpanel.h"
#include "dockcontainer.h"

#include <QBoxLayout>

QX_DOCK_BEGIN_NAMESPACE

class DockFloatingContainerPrivate
{
public:
    QX_DECLARE_PUBLIC(DockFloatingContainer)
public:
    DockFloatingContainerPrivate();
public:
    QPointer<DockWindow> m_window;
    DockContainer *m_dockContainer;
};

DockFloatingContainerPrivate::DockFloatingContainerPrivate()
{

}

DockFloatingContainer::DockFloatingContainer(DockWindow *window)
    : DockFloatingContainerBase(window)
{
    QX_INIT_PRIVATE(DockFloatingContainer)

    Q_D(DockFloatingContainer);
    d->m_window = window;
    d->m_dockContainer = new DockContainer(window, this);
    connect(d->m_dockContainer, SIGNAL(dockAreasAdded()), this,
        SLOT(onDockAreasAddedOrRemoved()));
    connect(d->m_dockContainer, SIGNAL(dockAreasRemoved()), this,
        SLOT(onDockAreasAddedOrRemoved()));

    QBoxLayout *l = new QBoxLayout(QBoxLayout::TopToBottom);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
    setLayout(l);
    l->addWidget(d->m_dockContainer);

    window->registerFloatingWidget(this);
}

DockFloatingContainer::DockFloatingContainer(DockPanel *panel)
    : DockFloatingContainer(panel->dockWindow())
{
    Q_D(DockFloatingContainer);
    d->m_dockContainer->addDockPanel(panel);
    d->m_window->notifyDockAreaRelocation(panel);
}

DockFloatingContainer::~DockFloatingContainer()
{
    QX_FINI_PRIVATE()
}

void DockFloatingContainer::onDockAreasAddedOrRemoved()
{

}

void QxDock::DockFloatingContainer::startFloating(const QPoint &dragStartMousePos, const QSize &size,
                                                  Qx::DockDragState dragState, QWidget *mouseEventHandler)
{

}

void DockFloatingContainer::finishDragging()
{

}

void DockFloatingContainer::moveFloating()
{

}

QX_DOCK_END_NAMESPACE
