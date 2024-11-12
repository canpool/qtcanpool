/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock_global.h"
#include <QWidget>

QX_DOCK_BEGIN_NAMESPACE

class DockWindow;
class DockContainer;
class DockPanel;
class DockTab;

class DockWidgetPrivate;

class QX_DOCK_EXPORT DockWidget : public QWidget
{
    Q_OBJECT
public:
    enum DockWidgetFeature {
        DockWidgetClosable = 0x001,
        DockWidgetMovable = 0x002,
        DockWidgetFloatable = 0x004,
        DockWidgetDeleteOnClose = 0x008,
        CustomCloseHandling = 0x010,
        DockWidgetForceCloseWithArea = 0x040,
        NoTab = 0x080,
        DefaultDockWidgetFeatures = DockWidgetClosable | DockWidgetMovable | DockWidgetFloatable,
        AllDockWidgetFeatures = DefaultDockWidgetFeatures | DockWidgetDeleteOnClose | CustomCloseHandling,
        DockWidgetAlwaysCloseAndDelete = DockWidgetForceCloseWithArea | DockWidgetDeleteOnClose,
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

public:
    explicit DockWidget(const QString &title, QWidget *parent = nullptr);
    ~DockWidget();

    DockTab *tab() const;

    DockWidgetFeatures features() const;

    DockWindow *dockWindow() const;
    DockContainer *dockContainer() const;
    DockPanel *dockPanel() const;

    bool isFloating() const;
    bool isInFloatingContainer() const;
    bool isClosed() const;

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

public Q_SLOTS:
    void toggleView(bool open = true);
    void deleteDockWidget();
    void requestCloseDockWidget();

Q_SIGNALS:
    void viewToggled(bool open);
    void closeRequested();
    void closed();
    void topLevelChanged(bool topLevel);

protected:
    void setDockWindow(DockWindow *window);
    void setDockPanel(DockPanel *panel);
    void setClosedState(bool closed);
    static void emitTopLevelEventForWidget(DockWidget *topLevelDockWidget, bool floating);
    void emitTopLevelChanged(bool floating);
    void toggleViewInternal(bool open);
    bool closeDockWidgetInternal(bool forceClose = false);

private:
    QX_DECLARE_PRIVATE(DockWidget)
    friend class DockContainer;
    friend class DockPanel;
    friend class DockWindow;
    friend class DockTitleBarPrivate;
    friend class DockFloatingContainer;
};

QX_DOCK_END_NAMESPACE
