/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include <QWidget>
#include "dockwidget.h"

class QSplitter;

QX_DOCK_BEGIN_NAMESPACE

class DockWindow;
class DockPanel;
class DockWidget;
class DockFloatingContainer;
class DockSplitter;

class DockContainerPrivate;

class QX_DOCK_EXPORT DockContainer : public QWidget
{
    Q_OBJECT
public:
    explicit DockContainer(DockWindow *window, QWidget *parent = nullptr);
    virtual ~DockContainer();

    DockPanel *addDockWidget(Qx::DockWidgetArea area, DockWidget *w, DockPanel *p = nullptr, int index = -1);
    void removeDockWidget(DockWidget *w);

    virtual unsigned int zOrderIndex() const;
    bool isInFrontOf(DockContainer *other) const;

    DockPanel *dockPanelAt(const QPoint &globalPos) const;
    DockPanel *dockPanel(int index) const;

    QList<DockPanel *> openedDockPanels() const;
    QList<DockWidget *> openedDockWidgets() const;
    bool hasTopLevelDockWidget() const;
    int dockPanelCount() const;
    int visibleDockPanelCount() const;

    DockWidget::DockWidgetFeatures features() const;
    bool isFloating() const;

    DockFloatingContainer *floatingWidget() const;

Q_SIGNALS:
    void dockAreasAdded();
    void dockAreasRemoved();

protected:
    DockSplitter *rootSplitter() const;
    void createRootSplitter();
    void addDockPanel(DockPanel *panel, Qx::DockWidgetArea area = Qx::CenterDockWidgetArea);
    void removeDockPanel(DockPanel *panel);
    QList<QPointer<DockPanel> > removeAllDockPanels();
    DockWidget *topLevelDockWidget() const;
    DockPanel *topLevelDockPanel() const;
    QList<DockWidget *> dockWidgets() const;
    void updateSplitterHandles(QSplitter *splitter);
    void dropFloatingWidget(DockFloatingContainer *floatingWidget, const QPoint &targetPos);

private:
    QX_DECLARE_PRIVATE(DockContainer)
    friend class DockPanel;
    friend class DockFloatingContainer;
    friend class DockFloatingContainerPrivate;
    friend class DockWidget;
};

QX_DOCK_END_NAMESPACE
