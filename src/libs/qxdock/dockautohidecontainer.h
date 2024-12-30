/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock_global.h"
#include <QFrame>

class QXmlStreamWriter;

QX_DOCK_BEGIN_NAMESPACE

class DockWidget;
class DockContainer;
class DockSideTab;
class DockSideBar;

class DockAutoHideContainerPrivate;

/**
 * Auto hide container for hosting an auto hide dock widget
 */
class QX_DOCK_EXPORT DockAutoHideContainer : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(int sideBarArea READ sideBarArea)
public:
    using Super = QFrame;
public:
    DockAutoHideContainer(DockWidget *w, Qx::DockSideBarArea area, DockContainer *parent);
    virtual ~DockAutoHideContainer();

    DockSideBar *autoHideSideBar() const;
    DockSideTab *autoHideTab() const;
    int tabIndex() const;

    DockWidget *dockWidget() const;
    void addDockWidget(DockWidget *w);

    DockContainer *dockContainer() const;

    Qx::DockSideBarArea sideBarArea() const;

    void cleanupAndDelete();
    void moveContentsToParent();
    void moveToNewSideBarArea(Qx::DockSideBarArea area, int tabIndex = -1);

    void toggleView(bool enable);
    void collapseView(bool enable);
    void toggleCollapseState();

    void setSize(int size);
    void resetToInitialDockWidgetSize();

    Qt::Orientation orientation() const;

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    virtual bool event(QEvent *event) override;

    void updateSize();
    void saveState(QXmlStreamWriter &s) const;

private:
    void setSideBarArea(Qx::DockSideBarArea area);

private:
    QX_DECLARE_PRIVATE(DockAutoHideContainer)
    friend class DockSideBar;
};

QX_DOCK_END_NAMESPACE
