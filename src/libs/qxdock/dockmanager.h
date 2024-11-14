/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include "dockiconprovider.h"

QX_DOCK_BEGIN_NAMESPACE

class DockManagerPrivate;

class QX_DOCK_EXPORT DockManager
{
public:
    enum ConfigFlag {
        ActiveTabHasCloseButton = 0x0001,
        DockAreaHasCloseButton = 0x0002,
        DockAreaCloseButtonClosesTab = 0x0004,
        OpaqueSplitterResize = 0x0008,
        TabCloseButtonIsToolButton = 0x0040,
        AllTabsHaveCloseButton = 0x0080,
        RetainTabSizeWhenCloseButtonHidden = 0x0100,
        DragPreviewIsDynamic = 0x0400,
        DragPreviewShowsContentPixmap = 0x0800,
        DragPreviewHasWindowFrame = 0x1000,
        AlwaysShowTabs = 0x2000,
        DockAreaHasUndockButton = 0x4000,
        DockAreaHasTabsMenuButton = 0x8000,
        DockAreaDynamicTabsMenuButtonVisibility = 0x20000,
        FloatingContainerHasWidgetTitle = 0x40000,
        FloatingContainerHasWidgetIcon = 0x80000,
        HideSingleCentralWidgetTitleBar = 0x100000,

        FocusHighlighting = 0x200000,
        EqualSplitOnInsertion = 0x400000,

        MiddleMouseButtonClosesTab = 0x2000000,
        DisableTabTextEliding =      0x4000000,
        ShowTabTextOnlyForActiveTab = 0x8000000,

        DefaultDockAreaButtons = DockAreaHasCloseButton
                               | DockAreaHasUndockButton
                               | DockAreaHasTabsMenuButton,

        DefaultBaseConfig = DefaultDockAreaButtons
                          | ActiveTabHasCloseButton
                          | FloatingContainerHasWidgetTitle,

        DefaultOpaqueConfig = DefaultBaseConfig
                            | OpaqueSplitterResize
                            | DragPreviewShowsContentPixmap,

        DefaultNonOpaqueConfig = DefaultBaseConfig
                               | DragPreviewShowsContentPixmap,
        NonOpaqueWithWindowFrame = DefaultNonOpaqueConfig
                                 | DragPreviewHasWindowFrame
    };
    Q_DECLARE_FLAGS(ConfigFlags, ConfigFlag)

    enum AutoHideFlag {
        AutoHideFeatureEnabled = 0x01,
        DockAreaHasAutoHideButton = 0x02,
        AutoHideButtonTogglesArea = 0x04,
        AutoHideButtonCheckable = 0x08,
        AutoHideSideBarsIconOnly = 0x10,
        AutoHideShowOnMouseOver = 0x20,
        AutoHideCloseButtonCollapsesDock = 0x40,
        AutoHideHasCloseButton = 0x80,
        AutoHideHasMinimizeButton = 0x100,

        DefaultAutoHideConfig = AutoHideFeatureEnabled
                              | DockAreaHasAutoHideButton
                              | AutoHideHasMinimizeButton
    };
    Q_DECLARE_FLAGS(AutoHideFlags, AutoHideFlag)

public:
    DockManager();
    ~DockManager();

    static ConfigFlags configFlags();
    static void setConfigFlags(const ConfigFlags flags);
    static void setConfigFlag(ConfigFlag flag, bool on = true);
    static bool testConfigFlag(ConfigFlag flag);

    static AutoHideFlags autoHideConfigFlags();
    static void setAutoHideConfigFlags(const AutoHideFlags flags);
    static void setAutoHideConfigFlag(AutoHideFlag flag, bool on = true);
    static bool testAutoHideConfigFlag(AutoHideFlag flag);

    static DockIconProvider &iconProvider();

    static int startDragDistance();

    template <class QWidgetPtr>
    static void setWidgetFocus(QWidgetPtr widget) {
        if (!DockManager::testConfigFlag(DockManager::FocusHighlighting)) {
            return;
        }
        widget->setFocus(Qt::OtherFocusReason);
    }

    static QString floatingContainersTitle();
    static void setFloatingContainersTitle(const QString &title);

private:
    QX_DECLARE_PRIVATE(DockManager)
};

QX_DOCK_END_NAMESPACE
