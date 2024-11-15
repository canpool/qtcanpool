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
    Q_PROPERTY(int sideBarArea READ sideBarArea)
    Q_PROPERTY(Qt::Orientation orientation READ orientation)
    Q_PROPERTY(bool activeTab READ isActiveTab)
    Q_PROPERTY(bool iconOnly READ iconOnly)
public:
    using Super = DockButton;
public:
    explicit DockSideTab(QWidget *parent = nullptr);
    virtual ~DockSideTab();

    DockWidget *dockWidget() const;
    void setDockWidget(DockWidget *w);

    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation orientation);

    void updateStyle();

    Qx::DockSideBarArea sideBarArea() const;
    bool isActiveTab() const;
    bool iconOnly() const;

    DockSideBar *sideBar() const;
    int tabIndex() const;

protected:
    void setSideBar(DockSideBar *sideBar);
    void removeFromSideBar();
    virtual bool event(QEvent *e) override;
    virtual void mousePressEvent(QMouseEvent *e) override;
    virtual void mouseReleaseEvent(QMouseEvent *e) override;
    virtual void mouseMoveEvent(QMouseEvent *e) override;

private:
    QX_DECLARE_PRIVATE(DockSideTab)
    friend class DockSideBar;
    friend class DockAutoHideContainer;
};

QX_DOCK_END_NAMESPACE
