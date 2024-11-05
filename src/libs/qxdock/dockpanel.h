/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock/dockwidget.h"
#include "qxdock_global.h"
#include <QWidget>

class QAbstractButton;

QX_DOCK_BEGIN_NAMESPACE

class DockWindow;
class DockContainer;
class DockSplitter;

class DockPanelPrivate;

class QX_DOCK_EXPORT DockPanel : public QWidget
{
    Q_OBJECT
public:
    explicit DockPanel(DockWindow *window, DockContainer *parent);
    ~DockPanel();

    DockWindow *dockWindow() const;
    DockContainer *dockContainer() const;

    DockSplitter *parentSplitter() const;

    int dockWidgetsCount() const;
    QList<DockWidget *> dockWidgets() const;
    DockWidget *dockWidget(int index) const;

    QAbstractButton *titleBarButton(Qx::DockTitleBarButton which) const;

    bool isCentralWidgetArea() const;

public Q_SLOTS:
    void setCurrentIndex(int index);
protected:
    void addDockWidget(DockWidget *w);
    void insertDockWidget(int index, DockWidget *w, bool activate = true);
private:
    QX_DECLARE_PRIVATE(DockPanel)
    friend class DockContainerPrivate;
};

QX_DOCK_END_NAMESPACE
