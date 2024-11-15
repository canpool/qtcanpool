/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock_global.h"
#include <QFrame>

class QToolBar;

QX_DOCK_BEGIN_NAMESPACE

class DockWindow;
class DockContainer;
class DockPanel;
class DockTab;
class DockSideTab;
class DockAutoHideContainer;
class DockFloatingContainer;

class DockWidgetPrivate;

class QX_DOCK_EXPORT DockWidget : public QFrame
{
    Q_OBJECT
public:
    using Super = QFrame;

    enum DockWidgetFeature {
        DockWidgetClosable = 0x001,
        DockWidgetMovable = 0x002,
        DockWidgetFloatable = 0x004,
        DockWidgetDeleteOnClose = 0x008,
        CustomCloseHandling = 0x010,
        DockWidgetFocusable = 0x020,
        DockWidgetForceCloseWithArea = 0x040,
        NoTab = 0x080,
        DeleteContentOnClose = 0x100,
        DockWidgetPinnable = 0x200,
        DefaultDockWidgetFeatures = DockWidgetClosable | DockWidgetMovable | DockWidgetFloatable
                                  | DockWidgetFocusable | DockWidgetPinnable,
        AllDockWidgetFeatures = DefaultDockWidgetFeatures | DockWidgetDeleteOnClose | CustomCloseHandling,
        DockWidgetAlwaysCloseAndDelete = DockWidgetForceCloseWithArea | DockWidgetDeleteOnClose,
        GloballyLockableFeatures = DockWidgetClosable | DockWidgetMovable | DockWidgetFloatable | DockWidgetPinnable,
        NoDockWidgetFeatures = 0x000
    };
    Q_DECLARE_FLAGS(DockWidgetFeatures, DockWidgetFeature)

    enum ToggleViewActionMode {
        ActionModeToggle,
        ActionModeShow
    };

    enum MinimumSizeHintMode {
        MinimumSizeHintFromDockWidget,
        MinimumSizeHintFromContent,
        MinimumSizeHintFromDockWidgetMinimumSize,
        MinimumSizeHintFromContentMinimumSize,
    };

    enum WidgetInsertMode {
        AutoScrollArea,
        ForceScrollArea,
        ForceNoScrollArea
    };

    enum State {
        StateHidden,
        StateDocked,
        StateFloating
    };

    enum ToolBarStyleSource
    {
        ToolBarStyleFromDockWindow,
        ToolBarStyleFromDockWidget
    };

public:
    explicit DockWidget(const QString &title, QWidget *parent = nullptr);
    ~DockWidget();

    void setWidget(QWidget *w, WidgetInsertMode insertMode = AutoScrollArea);
    QWidget *widget() const;
    QWidget *takeWidget();

    DockTab *tab() const;

    DockWidgetFeatures features() const;
    void setFeatures(DockWidgetFeatures features);
    void setFeature(DockWidgetFeature flag, bool on);
    void notifyFeaturesChanged();

    DockWindow *dockWindow() const;
    DockContainer *dockContainer() const;
    DockFloatingContainer *dockFloatingContainer() const;
    DockPanel *dockPanel() const;

    bool isFloating() const;
    bool isInFloatingContainer() const;
    bool isClosed() const;
    bool isFullScreen() const;
    bool isTabbed() const;
    bool isCurrentTab() const;
    bool isCentralWidget() const;

    QAction *toggleViewAction() const;
    void setToggleViewAction(QAction *action);
    void setToggleViewActionMode(ToggleViewActionMode mode);
    void setToggleViewActionChecked(bool checked);

    virtual QList<QAction *> titleBarActions() const;

    MinimumSizeHintMode minimumSizeHintMode() const;
    void setMinimumSizeHintMode(MinimumSizeHintMode mode);
    virtual QSize minimumSizeHint() const override;

    QIcon icon() const;
    void setIcon(const QIcon &icon);

    QToolBar *toolBar() const;
    void setToolBar(QToolBar *toolBar);

    ToolBarStyleSource toolBarStyleSource() const;
    void setToolBarStyleSource(ToolBarStyleSource source);
    Qt::ToolButtonStyle toolBarStyle(State state) const;
    void setToolBarStyle(Qt::ToolButtonStyle style, State state);
    QSize toolBarIconSize(State state) const;
    void setToolBarIconSize(const QSize &iconSize, State state);

    DockSideTab *sideTab() const;
    void setSideTab(DockSideTab *sideTab);
    bool isAutoHide() const;
    DockAutoHideContainer *autoHideContainer() const;
    Qx::DockSideBarArea autoHideArea() const;

public Q_SLOTS:
    void toggleView(bool open = true);
    void deleteDockWidget();
    void closeDockWidget();
    void requestCloseDockWidget();
    void setAutoHide(bool enable, Qx::DockSideBarArea area = Qx::DockSideBarNone, int tabIndex = -1);
    void toggleAutoHide(Qx::DockSideBarArea area = Qx::DockSideBarNone);
    void setAsCurrentTab();
    void raise();
    void showFullScreen();
    void showNormal();

private Q_SLOTS:
    void setToolBarFloatingStyle(bool floating);

Q_SIGNALS:
    void viewToggled(bool open);
    void closeRequested();
    void closed();
    void topLevelChanged(bool topLevel);
    void titleChanged(const QString &title);
    void visibilityChanged(bool visible);
    void featuresChanged(DockWidget::DockWidgetFeatures features);

protected:
    void setDockWindow(DockWindow *window);
    void setDockPanel(DockPanel *panel);
    void setClosedState(bool closed);
    static void emitTopLevelEventForWidget(DockWidget *topLevelDockWidget, bool floating);
    void emitTopLevelChanged(bool floating);
    void toggleViewInternal(bool open);
    bool closeDockWidgetInternal(bool forceClose = false);
    void flagAsUnassigned();

protected:
    virtual bool event(QEvent *e) override;

private:
    QX_DECLARE_PRIVATE(DockWidget)
    friend class DockContainer;
    friend class DockPanel;
    friend class DockWindow;
    friend class DockWindowPrivate;
    friend class DockTitleBarPrivate;
    friend class DockFloatingContainer;
    friend class DockAutoHideContainer;
};

QX_DOCK_END_NAMESPACE
