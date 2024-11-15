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
class DockTitleBar;
class DockAutoHideContainer;

class DockPanelPrivate;

class QX_DOCK_EXPORT DockPanel : public QWidget
{
    Q_OBJECT
public:
    enum DockAreaFlag
    {
        HideSingleWidgetTitleBar = 0x0001,
        DefaultFlags = 0x0000
    };
    Q_DECLARE_FLAGS(DockAreaFlags, DockAreaFlag)

public:
    explicit DockPanel(DockWindow *window, DockContainer *parent);
    ~DockPanel();

    DockWindow *dockWindow() const;
    DockContainer *dockContainer() const;

    DockSplitter *parentSplitter() const;

    QRect titleBarGeometry() const;

    int dockWidgetsCount() const;
    QList<DockWidget *> dockWidgets() const;
    int openDockWidgetsCount() const;
    QList<DockWidget *> openedDockWidgets() const;
    DockWidget *dockWidget(int index) const;

    int currentIndex() const;

    DockWidget *currentDockWidget() const;
    void setCurrentDockWidget(DockWidget *w);

    DockWidget::DockWidgetFeatures features(Qx::DockBitwiseOperator mode = Qx::DockBitwiseAnd) const;

    QAbstractButton *titleBarButton(Qx::DockTitleBarButton which) const;

    virtual void setVisible(bool visible) override;

    Qx::DockWidgetAreas allowedAreas() const;
    void setAllowedAreas(Qx::DockWidgetAreas areas);

    DockTitleBar *titleBar() const;

    bool isCentralWidgetArea() const;

    bool isTopLevelArea() const;

    DockAutoHideContainer *autoHideContainer() const;
    void setAutoHideContainer(DockAutoHideContainer *container);
    bool isAutoHide() const;

public Q_SLOTS:
    void setCurrentIndex(int index);
    void closeArea();
    void closeOtherAreas();
    void setAutoHide(bool enable, Qx::DockSideBarArea area = Qx::DockSideBarNone, int tabIndex = -1);
    void toggleAutoHide(Qx::DockSideBarArea area = Qx::DockSideBarNone);

protected Q_SLOTS:
    void toggleView(bool open);

private Q_SLOTS:
    void onTabCloseRequested(int index);
    void reorderDockWidget(int fromIndex, int toIndex);
    void updateAutoHideButtonCheckState();
    void updateTitleBarButtonsToolTips();
    Qx::DockSideBarArea calculateSideBarArea() const;
    void onDockWidgetFeaturesChanged();

Q_SIGNALS:
    void currentChanging(int index);
    void currentChanged(int index);
    void viewToggled(bool open);

protected:
    void addDockWidget(DockWidget *w);
    void insertDockWidget(int index, DockWidget *w, bool activate = true);
    void removeDockWidget(DockWidget *w);
    DockWidget *nextOpenDockWidget(DockWidget *w) const;
    void toggleDockWidgetView(DockWidget *w, bool open);
    int indexOf(DockWidget *w) const;
    void hideAreaWithNoVisibleContent();
    void updateTitleBarVisibility();
    void internalSetCurrentDockWidget(DockWidget *w);
    void updateTitleBarButtonVisibility(bool isTopLevel);
    void markTitleBarMenuOutdated();

private:
    QX_DECLARE_PRIVATE(DockPanel)
    friend class DockContainerPrivate;
    friend class DockWidget;
    friend class DockWidgetPrivate;
    friend class DockContainer;
    friend class DockAutoHideContainer;
    friend class DockWindow;
};

QX_DOCK_END_NAMESPACE
