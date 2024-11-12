/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock_global.h"
#include <QScrollArea>

QX_DOCK_BEGIN_NAMESPACE

class DockContainer;
class DockAutoHideContainer;
class DockWidget;
class DockSideTab;

class DockSideBarPrivate;

class QX_DOCK_EXPORT DockSideBar : public QScrollArea
{
    Q_OBJECT
public:
    explicit DockSideBar(DockContainer *parent, Qx::DockSideBarArea area);
    virtual ~DockSideBar();

    DockAutoHideContainer *insertDockWidget(int index, DockWidget *w);

    Qx::DockSideBarArea sideBarArea() const;

protected:
    void insertTab(int index, DockSideTab *sideTab);

private:
    QX_DECLARE_PRIVATE(DockSideBar)
};

QX_DOCK_END_NAMESPACE
