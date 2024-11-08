/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockwindow.h"
#include "dockpanel.h"
#include "dockwidget.h"
#include "dockfloatingcontainer.h"

QX_DOCK_BEGIN_NAMESPACE

class DockWindowPrivate
{
public:
    QX_DECLARE_PUBLIC(DockWindow)
public:
    DockWindowPrivate();
public:
    QList<DockContainer *> m_containers;
    DockWidget *m_centralWidget = nullptr;
    QList<QPointer<DockFloatingContainer> > m_floatingWidgets;
};

DockWindowPrivate::DockWindowPrivate()
{
}

DockWindow::DockWindow(QWidget *parent)
    : DockContainer{this, parent}
{
    QX_INIT_PRIVATE(DockWindow)
    createRootSplitter();
    registerDockContainer(this);
}

DockWindow::~DockWindow()
{
    QX_FINI_PRIVATE()
}

/**
 * Adds dockwidget into the given area.
 * If DockPanel is not null, then the area parameter indicates the area
 * into the DockPanel. If DockPanel is null, the Dockwidget will
 * be dropped into the container. If you would like to add a dock widget
 * tabified, then you need to add it to an existing dock panel object
 * into the CenterDockWidgetArea. The following code shows this:
 * \code
 * DockWindow->addDockWidget(Qx::CenterDockWidgetArea, newDockWidget,
 * 	   ExisitingPanel);
 * \endcode
 * \return Returns the dock panel that contains the new DockWidget
 */
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

void DockWindow::removeDockWidget(DockWidget *w)
{
    Q_D(DockWindow);
    Q_EMIT dockWidgetAboutToBeRemoved(w);
    DockContainer::removeDockWidget(w);
    w->setDockWindow(nullptr);
    Q_EMIT dockWidgetRemoved(w);
}

DockWidget *DockWindow::centralWidget() const
{
    Q_D(const DockWindow);
    return d->m_centralWidget;
}

void DockWindow::registerDockContainer(DockContainer *container)
{
    Q_D(DockWindow);
    d->m_containers.append(container);
}

void DockWindow::removeDockContainer(DockContainer *container)
{
    Q_D(DockWindow);
    if (this != container) {
        d->m_containers.removeAll(container);
    }
}

void DockWindow::registerFloatingWidget(DockFloatingContainer *floatingWidget)
{
    Q_D(DockWindow);
    d->m_floatingWidgets.append(floatingWidget);
    Q_EMIT floatingWidgetCreated(floatingWidget);
}

void DockWindow::notifyDockAreaRelocation(QWidget *relocatedWidget)
{

}

QX_DOCK_END_NAMESPACE
