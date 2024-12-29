/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include "dockiconprovider.h"

QX_DOCK_BEGIN_NAMESPACE

class DockManagerPrivate;

/**
 * The dock manager that maintains the complete docking system.
 * With the configuration flags you can globally control the functionality
 * of the docking system.
 **/
class QX_DOCK_EXPORT DockManager
{
public:
    /**
     * These global configuration flags configure some global dock manager settings.
     * Set the dock manager flags, before you create the dock manager instance.
     */
    enum ConfigFlag {
        // If this flag is set, the active tab in a tab area has a close button
        ActiveTabHasCloseButton = 0x0001,
        // If the flag is set each dock area has a close button
        DockAreaHasCloseButton = 0x0002,
        // If the flag is set, the dock area close button closes the active tab,
        // if not set, it closes the complete dock area
        DockAreaCloseButtonClosesTab = 0x0004,
        // See QSplitter::setOpaqueResize() documentation
        OpaqueSplitterResize = 0x0008,
        // If enabled, the XML writer automatically adds line-breaks and indentation to
        // empty sections between elements (ignorable whitespace).
        XmlAutoFormattingEnabled = 0x0010,
        // If enabled, the XML output will be compressed and is not human readable anymore
        XmlCompressionEnabled = 0x0020,
        // If enabled the tab close buttons will be QToolButtons instead of QPushButtons - disabled by default
        TabCloseButtonIsToolButton = 0x0040,
        // if this flag is set, then all tabs that are closable show a close button
        AllTabsHaveCloseButton = 0x0080,
        // if this flag is set, the space for the close button is reserved even if the close button is not visible
        RetainTabSizeWhenCloseButtonHidden = 0x0100,
        // If opaque undocking is disabled, this flag defines the behavior of the drag preview window,
        // if this flag is enabled, the preview will be adjusted dynamically to the drop area
        DragPreviewIsDynamic = 0x0400,
        // If opaque undocking is disabled, the created drag preview window shows a copy of the content
        // of the dock widget / dock are that is dragged
        DragPreviewShowsContentPixmap = 0x0800,
        // If opaque undocking is disabled, then this flag configures if the drag preview is frameless or
        // looks like a real window
        DragPreviewHasWindowFrame = 0x1000,
        // If this option is enabled, the tab of a dock widget is always displayed - even if
        // it is the only visible dock widget in a floating widget.
        AlwaysShowTabs = 0x2000,
        // If the flag is set each dock area has an undock button
        DockAreaHasUndockButton = 0x4000,
        // If the flag is set each dock area has a tabs menu button
        DockAreaHasTabsMenuButton = 0x8000,
        // If the flag is set disabled dock area buttons will not appear on the toolbar
        // at all (enabling them will bring them back)
        DockAreaHideDisabledButtons = 0x10000,
        // If the flag is set, the tabs menu button will be shown only when it is required - that means,
        // if the tabs are elided. If the tabs are not elided, it is hidden
        DockAreaDynamicTabsMenuButtonVisibility = 0x20000,
        // If set, the Floating Widget window title reflects the title of the current dock widget
        // otherwise it displays the title set with `DockManager::setFloatingContainersTitle` or
        // application name as window title
        FloatingContainerHasWidgetTitle = 0x40000,
        // If set, the Floating Widget icon reflects the icon of the current dock widget
        // otherwise it displays application icon
        FloatingContainerHasWidgetIcon = 0x80000,
        // If there is only one single visible dock widget in the main dock container (the dock manager)
        // and if this flag is set, then the titlebar of this dock widget will be hidden
        // this only makes sense for non draggable and non floatable widgets and
        // enables the creation of some kind of "central" widget
        HideSingleCentralWidgetTitleBar = 0x100000,

        // enables styling of focused dock widget tabs or floating widget titlebar
        FocusHighlighting = 0x200000,
        // if enabled, the space is equally distributed to all widgets in a  splitter
        EqualSplitOnInsertion = 0x400000,

        // Linux only ! Forces all FloatingContainer to use the native title bar.
        // This might break docking for FloatinContainer on some Window Managers (like Kwin/KDE).
        // If neither this nor FloatingContainerForceCustomTitleBar is set (the default) native titlebars
        // are used except on known bad systems.
        // Users can overwrite this by setting the environment variable ADS_UseNativeTitle to "1" or "0".
        FloatingContainerForceNativeTitleBar = 0x800000,
        // Linux only ! Forces all FloatingContainer to use a QWidget based title bar.
        // If neither this nor FloatingContainerForceNativeTitleBar is set (the default) native titlebars
        // are used except on known bad systems.
        // Users can overwrite this by setting the environment variable ADS_UseNativeTitle to "1" or "0".
        FloatingContainerForceQWidgetTitleBar = 0x1000000,

        // If the flag is set, the user can use the mouse middle button to close the tab under the mouse
        MiddleMouseButtonClosesTab = 0x2000000,
        // Set this flag to disable eliding of tab texts in dock area tabs
        DisableTabTextEliding =      0x4000000,
        // Set this flag to show label texts in dock area tabs only for active tabs
        ShowTabTextOnlyForActiveTab = 0x8000000,

        // default configuration of dock area title bar buttons
        DefaultDockAreaButtons = DockAreaHasCloseButton
                               | DockAreaHasUndockButton
                               | DockAreaHasTabsMenuButton,

        // default base configuration settings
        DefaultBaseConfig = DefaultDockAreaButtons
                          | ActiveTabHasCloseButton
                          | XmlCompressionEnabled
                          | FloatingContainerHasWidgetTitle,

        // the default configuration for opaque operations
        DefaultOpaqueConfig = DefaultBaseConfig
                            | OpaqueSplitterResize
                            | DragPreviewShowsContentPixmap,

        // the default configuration for non opaque operations
        DefaultNonOpaqueConfig = DefaultBaseConfig
                               | DragPreviewShowsContentPixmap,
        // the default configuration for non opaque operations that show a real window with frame
        NonOpaqueWithWindowFrame = DefaultNonOpaqueConfig
                                 | DragPreviewHasWindowFrame
    };
    Q_DECLARE_FLAGS(ConfigFlags, ConfigFlag)

    /**
     * These global configuration flags configure some dock manager auto hide settings
     * Set the dock manager flags, before you create the dock manager instance.
     */
    enum AutoHideFlag {
        // enables / disables auto hide feature
        AutoHideFeatureEnabled = 0x01,
        // If the flag is set each dock area has a auto hide menu button
        DockAreaHasAutoHideButton = 0x02,
        // If the flag is set, the auto hide button enables auto hiding for all dock widgets in an area,
        // if disabled, only the current dock widget will be toggled
        AutoHideButtonTogglesArea = 0x04,
        // If the flag is set, the auto hide button will be checked and unchecked depending on the auto hide state.
        // Mainly for styling purposes.
        AutoHideButtonCheckable = 0x08,
        // show only icons in auto hide side tab - if a tab has no icon, then the text will be shown
        AutoHideSideBarsIconOnly = 0x10,
        // show the auto hide window on mouse over tab and hide it if mouse leaves auto hide container
        AutoHideShowOnMouseOver = 0x20,
        // Close button of an auto hide container collapses the dock instead of hiding it completely
        AutoHideCloseButtonCollapsesDock = 0x40,
        // If the flag is set an auto hide title bar has a close button
        AutoHideHasCloseButton = 0x80,
        // if this flag is set, the auto hide title bar has a minimize button to collapse the dock widget
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

    // Helper function to set focus depending on the configuration of the FocusHighlighting flag
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
