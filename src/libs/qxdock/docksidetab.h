/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock_global.h"
#include "dockbutton.h"

QX_DOCK_BEGIN_NAMESPACE

class DockSideBar;
class DockWidget;

class DockSideTabPrivate;

class QX_DOCK_EXPORT DockSideTab : public DockButton
{
    Q_OBJECT
public:
    explicit DockSideTab(QWidget *parent = nullptr);
    virtual ~DockSideTab();

    DockWidget *dockWidget() const;
    void setDockWidget(DockWidget *w);

    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation orientation);

    void updateStyle();

protected:
    void setSideBar(DockSideBar *sideBar);

private:
    QX_DECLARE_PRIVATE(DockSideTab)
    friend class DockSideBar;
};

QX_DOCK_END_NAMESPACE
