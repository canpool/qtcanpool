/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock_global.h"
#include <QFrame>
#include <functional>

class QToolBar;
class QXmlStreamWriter;

QX_DOCK_BEGIN_NAMESPACE

class DockWindow;
class DockContainer;
class DockPanel;
class DockTab;
class DockSideTab;
class DockAutoHideContainer;
class DockFloatingContainer;

class DockWidgetPrivate;

/**
 * The DockWidget class provides a widget that can be docked inside a
 * DockWindow or floated as a top-level window on the desktop.
 */
class QX_DOCK_EXPORT DockWidget : public QFrame
{
    Q_OBJECT
public:
    using Super = QFrame;

    enum DockWidgetFeature {
        // dock widget has a close button
        DockWidgetClosable = 0x001,
        // dock widget is movable and can be moved to a new position in the current dock container
        DockWidgetMovable = 0x002,
        // dock widget can be dragged into a floating window
        DockWidgetFloatable = 0x004,
        // deletes the dock widget when it is closed
        DockWidgetDeleteOnClose = 0x008,
        // clicking the close button will not close the dock widget but emits the closeRequested() signal instead
        CustomCloseHandling = 0x010,
        // if this is enabled, a dock widget can get focus highlighting
        DockWidgetFocusable = 0x020,
        // dock widget will be closed when the dock area hosting it is closed
        DockWidgetForceCloseWithArea = 0x040,
        // dock widget tab will never be shown if this flag is set
        NoTab = 0x080,
        // deletes only the contained widget on close, keeping the dock widget intact and in place.
        // Attempts to rebuild the contents widget on show if there is a widget factory set.
        DeleteContentOnClose = 0x100,
        // dock widget can be pinned and added to an auto hide dock container
        DockWidgetPinnable = 0x200,

        DefaultDockWidgetFeatures = DockWidgetClosable | DockWidgetMovable | DockWidgetFloatable
                                  | DockWidgetFocusable | DockWidgetPinnable,
        AllDockWidgetFeatures = DefaultDockWidgetFeatures | DockWidgetDeleteOnClose | CustomCloseHandling,
        DockWidgetAlwaysCloseAndDelete = DockWidgetForceCloseWithArea | DockWidgetDeleteOnClose,
        GloballyLockableFeatures = DockWidgetClosable | DockWidgetMovable | DockWidgetFloatable | DockWidgetPinnable,
        NoDockWidgetFeatures = 0x000
    };
    Q_DECLARE_FLAGS(DockWidgetFeatures, DockWidgetFeature)
    /**
     * This mode configures the behavior of the toggle view action.
     * If the mode if ActionModeToggle, then the toggle view action is
     * a checkable action to show / hide the dock widget. If the mode
     * is ActionModeShow, then the action is not checkable an it will
     * always show the dock widget if clicked. If the mode is ActionModeShow,
     * the user can only close the DockWidget with the close button.
     */
    enum ToggleViewActionMode {
        ActionModeToggle,
        ActionModeShow
    };
    /**
     * The mode of the minimumSizeHint() that is returned by the DockWidget
     * minimumSizeHint() function.
     * To ensure, that a dock widget does not block resizing, the dock widget
     * reimplements minimumSizeHint() function to return a very small minimum
     * size hint. If you would like to adhere the minimumSizeHint() from the
     * content widget, then set the minimumSizeHintMode() to
     * MinimumSizeHintFromContent. If you would like to use the minimumSize()
     * value of the content widget or the dock widget, then you can use the
     * MinimumSizeHintFromDockWidgetMinimumSize or
     * MinimumSizeHintFromContentMinimumSize modes.
     */
    enum MinimumSizeHintMode {
        MinimumSizeHintFromDockWidget,
        MinimumSizeHintFromContent,
        MinimumSizeHintFromDockWidgetMinimumSize,
        MinimumSizeHintFromContentMinimumSize,
    };

    /**
     * Sets the widget for the dock widget to widget.
     * The InsertMode defines how the widget is inserted into the dock widget.
     * The content of a dock widget should be resizable do a very small size to
     * prevent the dock widget from blocking the resizing. To ensure, that a
     * dock widget can be resized very well, it is better to insert the content+
     * widget into a scroll area or to provide a widget that is already a scroll
     * area or that contains a scroll area.
     * If the InsertMode is AutoScrollArea, the DockWidget tries to automatically
     * detect how to insert the given widget. If the widget is derived from
     * QScrollArea (i.e. an QAbstractItemView), then the widget is inserted
     * directly. If the given widget is not a scroll area, the widget will be
     * inserted into a scroll area.
     * To force insertion into a scroll area, you can also provide the InsertMode
     * ForceScrollArea. To prevent insertion into a scroll area, you can
     * provide the InsertMode ForceNoScrollArea
     */
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
    void setTitleBarActions(QList<QAction *> actions);

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

    using FactoryFunc = std::function<QWidget *(QWidget *)>;
    void setWidgetFactory(FactoryFunc createWidget, WidgetInsertMode insertMode = AutoScrollArea);

#ifndef QT_NO_TOOLTIP
    void setTabToolTip(const QString &text);
#endif

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
    void setFloating();

private Q_SLOTS:
    void setToolBarFloatingStyle(bool floating);

Q_SIGNALS:
    void viewToggled(bool open);
    void closeRequested();
    void closed();
    /**
     * This signal is emitted when the floating property changes.
     * The topLevel parameter is true if the dock widget is now floating;
     * otherwise it is false.
     */
    void topLevelChanged(bool topLevel);
    void titleChanged(const QString &title);
    /**
     * This signal is emitted when the dock widget becomes visible (or invisible).
     * This happens when the widget is hidden or shown, as well as when it is
     * docked in a tabbed dock panel and its tab becomes selected or unselected.
     */
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
    void saveState(QXmlStreamWriter &s) const;

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
