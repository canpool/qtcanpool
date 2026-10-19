##
## Copyright (C) 2026 maminjie <canpool@163.com>
## SPDX-License-Identifier: MulanPSL-2.0
##

# Every sample is one main window - occasionally with a small extra class - and
# all of them share the single main() in this directory. What a sample does not
# always bring along is MainWindow itself, so `window` says where that comes from:
#
#   sample - the sample directory ships mainwindow.h
#   ribbon - it comes from qxribbon/shared/, and opens a RibbonMainWindow
#   plain  - it comes from shared/, and opens a plain QMainWindow
#   none   - the sample directory ships main() too, so nothing is shared with it
#
# The sample directory always comes first on the include path, so a sample that
# ships its own mainwindow.h wins over either shared one, and main.cpp - which
# does `#include "mainwindow.h"` - is resolved by that same order. That is why
# main.cpp sits here and not in shared/: a quoted include is looked for in the
# including file's own directory first, so a main.cpp living next to
# shared/mainwindow.h could only ever see that one, whatever the include path
# said. Keep it where no mainwindow.h shares its directory.
#
# The header supplying MainWindow has to be in the source list, because AUTOMOC
# only looks at headers that belong to the target - left out, its Q_OBJECT gets
# no moc_ file and the link fails on a missing vtable. Exactly one of them has to
# be there, because two MainWindow classes in one target is two staticMetaObject
# definitions - and it has to be the same header the sample's own mainwindow.cpp
# sees, or main() builds an object of the wrong size on the stack.
#
# The resource file is shared by all of them - /logo is its only alias. A sample
# that needs resources beyond that one lists them after the four fixed arguments.

function(add_qtc_example name library directory window)
    file(GLOB _sample_sources CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/${directory}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/${directory}/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/${directory}/*.ui")

    set(_sources ${_sample_sources})
    set(_includes "${CMAKE_CURRENT_SOURCE_DIR}/${directory}")
    set(_defines "")
    set(_libraries "")

    if (window STREQUAL "none")
        # Nothing shared to add.
    elseif (window STREQUAL "sample")
        list(APPEND _sources "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
    elseif (window STREQUAL "ribbon")
        list(APPEND _sources
            "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/qxribbon/shared/mainwindow.h")
        list(APPEND _includes "${CMAKE_CURRENT_SOURCE_DIR}/qxribbon/shared")
    elseif (window STREQUAL "plain")
        list(APPEND _sources
            "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/shared/mainwindow.h")
        list(APPEND _includes "${CMAKE_CURRENT_SOURCE_DIR}/shared")
    else()
        message(FATAL_ERROR "add_qtc_example(${name}): unknown window kind '${window}'")
    endif()

    list(APPEND _sources ${ARGN})

    # A sample built against a library that has no look of its own needs one:
    # qxribbon's sheets are the theme sheets, qxdock's are one part of them, and
    # neither library installs anything. Without a theme the sample opens on
    # whatever the platform style leaves behind, which is not what either
    # library is meant to look like and not what the sample is there to show.
    #
    # The bundle is registered by shared/themestart.cpp, called from the shared
    # main() - or from the sample's own, for the three that carry one. qxplugin
    # is left out on purpose: it has no widgets a theme would reach. A sample
    # that already builds res/themes.qrc registers the bundle itself, and one
    # target with two copies of the same qrc is two definitions of the same
    # init function.
    set(_extras ${ARGN})
    set(_want_theme FALSE)
    if (library STREQUAL "qxribbon" OR library STREQUAL "qxdock" OR library STREQUAL "qxapp")
        set(_want_theme TRUE)
    endif()
    foreach (_extra IN LISTS _extras)
        if (_extra MATCHES "themes\\.qrc$")
            set(_want_theme FALSE)
        endif()
    endforeach()

    if (_want_theme)
        list(APPEND _sources
            "${CMAKE_CURRENT_SOURCE_DIR}/shared/themestart.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/shared/themestart.h"
            "${PROJECT_SOURCE_DIR}/res/themes.qrc")
        # Last on the include path, so a mainwindow.h the sample ships - or the
        # ribbon one a ribbon sample is given - still wins over the plain one
        # that lives here.
        list(APPEND _includes "${CMAKE_CURRENT_SOURCE_DIR}/shared")
        list(APPEND _libraries qxtheme)
        list(APPEND _defines "QTC_EXAMPLE_THEME")
    endif()

    if (library STREQUAL "qxdock")
        if (NOT _want_theme)
            # A theme carries the dock's rules, so only an unthemed sample needs
            # the library's own sheet: without it a DockWindow is a row of
            # unstyled widgets. It lives in res/dockstyle.qrc and is offered the
            # way res/themes.qrc is - built in by whoever wants it. Doing it
            # here rather than in twelve sample directories is what keeps "add a
            # new dock sample" from being a two-step affair, where the second
            # step is the one nobody does.
            list(APPEND _sources "${PROJECT_SOURCE_DIR}/res/dockstyle.qrc")
        endif()
        # ... and the shared main() applies it, which it can only do behind a
        # define: a ribbon sample compiles the same main.cpp without qxdock to
        # link against. A themed dock sample gets this define too, because the
        # platform's piece of the dock's look is the one part a theme does not
        # carry.
        list(APPEND _defines "QTC_EXAMPLE_DOCK")
    endif()

    add_qtc_executable(${name}
        # Samples are built here and stay here; none of them belongs in an
        # installed SDK tree next to the libraries.
        SKIP_INSTALL
        DEFINES
            ${_defines}
        DEPENDS
            ${library}
            ${_libraries}
            Qt5::Core
            Qt5::Gui
            Qt5::Widgets
        INCLUDES
            ${_includes}
        SOURCES
            ${_sources}
            "${CMAKE_CURRENT_SOURCE_DIR}/shared/res.qrc"
    )
endfunction()

# ------------------------------------------------------------------ qxplugin --
# On-demand plugin loading (4.5): resolvePlugins() reads and orders metadata,
# requestPlugin() starts one plugin - with its still-needed dependencies - on ask.
add_qtc_example(Example_Plugin_LazyLoading qxplugin qxplugin/lazyloading none)

# ------------------------------------------------------------------ qxribbon --
add_qtc_example(Example_Ribbon_Simple qxribbon qxribbon/simple ribbon)
add_qtc_example(Example_Ribbon_GroupAutoLayout qxribbon qxribbon/groupautolayout sample)
add_qtc_example(Example_Ribbon_CustomizeDialogModeless qxribbon qxribbon/customizedialogmodeless ribbon)
add_qtc_example(Example_Ribbon_CustomizeWidget qxribbon qxribbon/customizewidget sample)
add_qtc_example(Example_Ribbon_PageContext qxribbon qxribbon/pagecontext ribbon)
add_qtc_example(Example_Ribbon_Menu qxribbon qxribbon/ribbonmenu sample)
add_qtc_example(Example_RibbonBar qxribbon qxribbon/ribbonbar sample)
add_qtc_example(Example_Ribbon_ButtonGroup qxribbon qxribbon/ribbonbuttongroup ribbon)
add_qtc_example(Example_Ribbon_Gallery qxribbon qxribbon/ribbongallery ribbon)
# 4.5: group-level shrink on resize (B11), with a live isReduced() readout.
add_qtc_example(Example_Ribbon_GroupShrink qxribbon qxribbon/groupshrink sample)
add_qtc_example(Example_Ribbon_MdiTabs qxribbon qxribbon/mditabs sample)
# Carries its own themes: the style sheets live in res/themes.qrc, the sample
# registers them with the engine, and qxapp is what pulls qxtheme in.
add_qtc_example(Example_RibbonStyle qxapp qxribbon/ribbonstyle sample
    "${CMAKE_SOURCE_DIR}/res/themes.qrc")
# 4.5: the full-takeover backstage the application button opens (B4).
add_qtc_example(Example_Ribbon_Backstage qxribbon qxribbon/backstage sample)
# Segments on the left, view switches on the right, in a themed status bar.
add_qtc_example(Example_Ribbon_StatusBar qxribbon qxribbon/statusbar sample)
# These two build against the generic main window in shared/.
add_qtc_example(Example_Ribbon_ColorIcon qxribbon qxribbon/coloricon plain)
add_qtc_example(Example_RibbonQuickAccessBar qxribbon qxribbon/ribbonquickaccessbar plain)

# -------------------------------------------------------------------- qxdock --
add_qtc_example(Example_Dock_AutoHide qxdock qxdock/autohide sample)
add_qtc_example(Example_Dock_CentralWidget qxdock qxdock/centralwidget sample)
add_qtc_example(Example_Dock_DockInDock qxdock qxdock/dockindock sample)
add_qtc_example(Example_Dock_Group qxdock qxdock/dockgroup sample)
add_qtc_example(Example_Dock_HideShow qxdock qxdock/hideshow sample)
add_qtc_example(Example_Dock_SideBar qxdock qxdock/sidebar sample)
# 4.5: plain, human-readable XML layout persistence (C2).
add_qtc_example(Example_Dock_Persistence qxdock qxdock/persistence sample)
# 4.5: native OS drag of a dock widget + the dockWidgetDropped() signal (C3).
add_qtc_example(Example_Dock_NativeDrag qxdock qxdock/dragdrop sample)
add_qtc_example(Example_Dock_ConfigFlags qxdock qxdock/configflags plain)
add_qtc_example(Example_Dock_EmptyDockArea qxdock qxdock/emptydockarea plain)
add_qtc_example(Example_Dock_WidgetToPanel qxdock qxdock/widgettopanel plain)
# The one sample that carries its own main().
add_qtc_example(Example_Dock_DeleteOnClose qxdock qxdock/deleteonclose none)
# A frameless host, so also its own main().
add_qtc_example(Example_Dock_Frameless qxdock qxdock/frameless none)
# Toolbars come from the host QMainWindow, so this one brings its own main() too.
add_qtc_example(Example_Dock_ToolBars qxdock qxdock/toolbars none)

# --------------------------------------------------------------------- qxapp --
add_qtc_example(Example_RibbonAppEmpty qxapp qxapp/ribbonappempty sample)
# 4.5: the smallest complete shell, built from the D1/D2 convenience calls.
add_qtc_example(Example_App_Shell qxapp qxapp/shell sample)
# 4.4.1: examples for the 11 classes that had no visual sample yet, grouped by family.
add_qtc_example(Example_App_PropertyEditor qxapp qxapp/propertyeditor sample)
add_qtc_example(Example_App_Buttons qxapp qxapp/buttons sample)
add_qtc_example(Example_App_NavigationBar qxapp qxapp/navigationbar sample)
add_qtc_example(Example_App_TabWidget qxapp qxapp/tabwidget sample)
