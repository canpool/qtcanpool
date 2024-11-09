/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockwindow.h"
#include "dockpanel.h"
#include "dockwidget.h"
#include "dockfloatingcontainer.h"
#include "dockoverlay.h"

QX_DOCK_BEGIN_NAMESPACE

class DockWindowPrivate
{
public:
    QX_DECLARE_PUBLIC(DockWindow)
public:
    DockWindowPrivate();

    void init();

public:
    QList<DockContainer *> m_containers;
    DockWidget *m_centralWidget = nullptr;
    QList<QPointer<DockFloatingContainer> > m_floatingContainers;
    DockOverlay *m_containerOverlay;
    DockOverlay *m_panelOverlay;
};

DockWindowPrivate::DockWindowPrivate()
{
}

void DockWindowPrivate::init()
{
    Q_Q(DockWindow);

    m_containerOverlay = new DockOverlay(q, DockOverlay::PanelOverlayMode);
    m_panelOverlay = new DockOverlay(q, DockOverlay::ContainerOverlayMode);
}

DockWindow::DockWindow(QWidget *parent)
    : DockContainer{this, parent}
{
    QX_INIT_PRIVATE(DockWindow)
    createRootSplitter();
    Q_D(DockWindow);
    d->init();
    registerDockContainer(this);
}

DockWindow::~DockWindow()
{
    Q_D(DockWindow);
    std::vector<QPointer<DockPanel> > panels;
    for (int i = 0; i < dockPanelCount(); ++i) {
        panels.push_back(dockPanel(i));
    }
    for (auto p : panels) {
        if (!p || p->dockWindow() != this) {
            continue;
        }
        std::vector<QPointer<QWidget> > deleteWidgets;
        for (auto w : p->dockWidgets()) {
            deleteWidgets.push_back(w);
        }
        for (auto w : deleteWidgets) {
            delete w;
        }
    }

    auto floatingContainers = d->m_floatingContainers;
    for (auto c : floatingContainers) {
        c->deleteContent();
        delete c;
    }

    for (auto p : panels) {
        delete p;
    }

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

const QList<DockContainer *> DockWindow::dockContainers() const
{
    Q_D(const DockWindow);
    return d->m_containers;
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
    d->m_floatingContainers.append(floatingWidget);
    Q_EMIT floatingWidgetCreated(floatingWidget);
}

void DockWindow::removeFloatingWidget(DockFloatingContainer *floatingWidget)
{
    Q_D(DockWindow);
    d->m_floatingContainers.removeAll(floatingWidget);
}

DockOverlay *DockWindow::containerOverlay() const
{
    Q_D(const DockWindow);
    return d->m_containerOverlay;
}

DockOverlay *DockWindow::panelOverlay() const
{
    Q_D(const DockWindow);
    return d->m_panelOverlay;
}

void DockWindow::notifyDockAreaRelocation(QWidget *relocatedWidget)
{

}

QX_DOCK_END_NAMESPACE
