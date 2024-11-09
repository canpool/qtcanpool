/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock_global.h"
#include "dockcontainer.h"

QX_DOCK_BEGIN_NAMESPACE

class DockPanel;
class DockWidget;
class DockFloatingContainer;
class DockOverlay;
class DockContainer;

class DockWindowPrivate;

class QX_DOCK_EXPORT DockWindow : public DockContainer
{
    Q_OBJECT
public:
    explicit DockWindow(QWidget *parent = nullptr);
    ~DockWindow();

    DockPanel *addDockWidget(Qx::DockWidgetArea area, DockWidget *w, DockPanel *p = nullptr, int index = -1);
    void removeDockWidget(DockWidget *w);

    const QList<DockContainer *> dockContainers() const;

    DockWidget *centralWidget() const;

Q_SIGNALS:
    void dockWidgetAdded(DockWidget *w);
    void dockWidgetAboutToBeRemoved(DockWidget *w);
    void dockWidgetRemoved(DockWidget *w);
    void floatingWidgetCreated(DockFloatingContainer *floatingWidget);

protected:
    void registerDockContainer(DockContainer *container);
    void removeDockContainer(DockContainer *container);
    void registerFloatingWidget(DockFloatingContainer *floatingWidget);
    void removeFloatingWidget(DockFloatingContainer *floatingWidget);

    DockOverlay *containerOverlay() const;
    DockOverlay *panelOverlay() const;

    void notifyDockAreaRelocation(QWidget *relocatedWidget);

private:
    QX_DECLARE_PRIVATE(DockWindow)
    friend class DockContainer;
    friend class DockFloatingContainer;
    friend class DockFloatingContainerPrivate;
};

QX_DOCK_END_NAMESPACE
