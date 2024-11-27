/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock_global.h"
#include "dockcontainer.h"
#include "dockwidget.h"

class QSettings;

QX_DOCK_BEGIN_NAMESPACE

class DockPanel;
class DockWidget;
class DockFloatingContainer;
class DockOverlay;
class DockContainer;
class DockFocusController;
class DockAutoHideContainer;

class DockWindowPrivate;

class QX_DOCK_EXPORT DockWindow : public DockContainer
{
    Q_OBJECT
public:
    using Super = DockContainer;
public:
    explicit DockWindow(QWidget *parent = nullptr);
    virtual ~DockWindow();

    DockPanel *addDockWidget(Qx::DockWidgetArea area, DockWidget *w, DockPanel *p = nullptr, int index = -1);
    DockPanel *addDockWidgetTab(Qx::DockWidgetArea area, DockWidget *w);
    DockPanel *addDockWidgetTab(DockWidget *w, DockPanel *p, int index = 1);
    DockPanel *addDockWidgetToContainer(Qx::DockWidgetArea area, DockWidget *w, DockContainer *container);
    void removeDockWidget(DockWidget *w);
    DockWidget *findDockWidget(const QString &objectName) const;

    DockAutoHideContainer *addAutoHideDockWidget(Qx::DockSideBarArea area, DockWidget *w);
    DockAutoHideContainer *addAutoHideDockWidgetToContainer(Qx::DockSideBarArea area,
                                                            DockWidget *w, DockContainer *container);

    DockFloatingContainer *addDockWidgetFloating(DockWidget *w);

    const QList<DockContainer *> dockContainers() const;
    QMap<QString, DockWidget *> dockWidgetsMap() const;

    DockWidget *centralWidget() const;
    DockPanel *setCentralWidget(DockWidget *widget);

    bool isLeavingMinimizedState() const;

    Qt::ToolButtonStyle dockWidgetToolBarStyle(DockWidget::State state) const;
    void setDockWidgetToolBarStyle(Qt::ToolButtonStyle style, DockWidget::State state);
    QSize dockWidgetToolBarIconSize(DockWidget::State state) const;
    void setDockWidgetToolBarIconSize(const QSize &iconSize, DockWidget::State state);

    DockWidget::DockWidgetFeatures globallyLockedDockWidgetFeatures() const;
    void lockDockWidgetFeaturesGlobally(DockWidget::DockWidgetFeatures features = DockWidget::GloballyLockableFeatures);

    QList<int> splitterSizes(DockPanel *panel) const;
    void setSplitterSizes(DockPanel *panel, const QList<int> &sizes);

    QByteArray saveState(int version = 0) const;
    bool restoreState(const QByteArray &state, int version = 0);
    bool isRestoringState() const;

    void addPerspective(const QString &uniquePrespectiveName);
    void removePerspective(const QString &name);
    void removePerspectives(const QStringList &names);
    QStringList perspectiveNames() const;
    void savePerspectives(QSettings &settings) const;
    void loadPerspectives(QSettings &settings);

Q_SIGNALS:
    void dockWidgetAdded(DockWidget *w);
    void dockWidgetAboutToBeRemoved(DockWidget *w);
    void dockWidgetRemoved(DockWidget *w);
    void floatingWidgetCreated(DockFloatingContainer *floatingWidget);
    void focusedDockWidgetChanged(DockWidget *old, DockWidget *now);
    void restoringState();
    void stateRestored();
    void perspectiveListChanged();
    void perspectiveListLoaded();
    void perspectivesRemoved();
    void openingPerspective(const QString &perspectiveName);
    void perspectiveOpened(const QString &perspectiveName);

public Q_SLOTS:
    void setDockWidgetFocused(DockWidget *w);
    void endLeavingMinimizedState();
    void openPerspective(const QString &perspectiveName);
    void hideWindowAndFloatingWidgets();

protected:
    void registerDockContainer(DockContainer *container);
    void removeDockContainer(DockContainer *container);
    void registerFloatingWidget(DockFloatingContainer *floatingWidget);
    void removeFloatingWidget(DockFloatingContainer *floatingWidget);

    DockOverlay *containerOverlay() const;
    DockOverlay *panelOverlay() const;

    void notifyDockAreaRelocation(QWidget *droppedWidget);
    void notifyFloatingWidgetDrop(DockFloatingContainer *floatingWidget);

    DockFocusController *dockFocusController() const;

    void restoreHiddenFloatingWidgets();

protected:
    bool eventFilter(QObject *obj, QEvent *e) override;
    virtual void showEvent(QShowEvent *e) override;

private:
    QX_DECLARE_PRIVATE(DockWindow)
    friend class DockContainer;
    friend class DockContainerPrivate;
    friend class DockFloatingContainer;
    friend class DockFloatingContainerPrivate;
    friend class DockTitleBar;
    friend class DockFloatingPreview;
    friend class DockFloatingPreviewPrivate;
    friend class DockTabPrivate;
    friend class DockSideBar;
    friend class DockSideTabPrivate;
};

QX_DOCK_END_NAMESPACE
