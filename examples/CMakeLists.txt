##
## Copyright (C) 2026 maminjie <canpool@163.com>
## SPDX-License-Identifier: MulanPSL-2.0
##

# Each sample is one main window - occasionally with a small extra class - plus
# a shared main() that used to arrive through an `include(../shared/shared.pri)`.
# Two kinds of shared main() exist, one per library group and one at the top
# level, and which of them a sample picks decides where its mainwindow.h comes
# from, so every target gets both include paths: the sample directory first,
# because a sample that ships its own mainwindow.h must win over the generic
# one in shared/, which is exactly what the samples without a header rely on.
#
# The resource file is shared by all of them - /logo is its only alias.

function(add_qtc_example name library directory main_source)
    file(GLOB _sample_sources CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/${directory}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/${directory}/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/${directory}/*.ui")

    set(_sources ${_sample_sources})
    if (NOT main_source STREQUAL "none")
        list(APPEND _sources "${CMAKE_CURRENT_SOURCE_DIR}/${main_source}")

        # A sample that ships no mainwindow.h of its own compiles against the
        # generic one in shared/, and AUTOMOC only looks at headers that belong
        # to the target - left out, its Q_OBJECT gets no moc_ file and the link
        # fails on a missing vtable. Adding it here, and only here, also keeps
        # it away from the samples that do bring their own MainWindow class.
        get_filename_component(_main_dir "${main_source}" DIRECTORY)
        if (_main_dir STREQUAL "shared")
            list(APPEND _sources "${CMAKE_CURRENT_SOURCE_DIR}/shared/mainwindow.h")
        endif()
    endif()

    add_qtc_executable(${name}
        # Samples are built here and stay here; none of them belongs in an
        # installed SDK tree next to the libraries.
        SKIP_INSTALL
        DEPENDS
            ${library}
            Qt5::Core
            Qt5::Gui
            Qt5::Widgets
        INCLUDES
            "${CMAKE_CURRENT_SOURCE_DIR}/${directory}"
            "${CMAKE_CURRENT_SOURCE_DIR}/shared"
        SOURCES
            ${_sources}
            "${CMAKE_CURRENT_SOURCE_DIR}/shared/res.qrc"
    )
endfunction()

# ------------------------------------------------------------------ qxribbon --
add_qtc_example(Example_Ribbon_Simple qxribbon qxribbon/simple qxribbon/shared/main.cpp)
add_qtc_example(Example_Ribbon_GroupAutoLayout qxribbon qxribbon/groupautolayout qxribbon/shared/main.cpp)
add_qtc_example(Example_Ribbon_CustomizeDialogModeless qxribbon qxribbon/customizedialogmodeless qxribbon/shared/main.cpp)
add_qtc_example(Example_Ribbon_CustomizeWidget qxribbon qxribbon/customizewidget qxribbon/shared/main.cpp)
add_qtc_example(Example_Ribbon_PageContext qxribbon qxribbon/pagecontext qxribbon/shared/main.cpp)
add_qtc_example(Example_Ribbon_Menu qxribbon qxribbon/ribbonmenu qxribbon/shared/main.cpp)
add_qtc_example(Example_RibbonBar qxribbon qxribbon/ribbonbar qxribbon/shared/main.cpp)
add_qtc_example(Example_Ribbon_ButtonGroup qxribbon qxribbon/ribbonbuttongroup qxribbon/shared/main.cpp)
add_qtc_example(Example_Ribbon_Gallery qxribbon qxribbon/ribbongallery qxribbon/shared/main.cpp)
add_qtc_example(Example_RibbonStyle qxribbon qxribbon/ribbonstyle qxribbon/shared/main.cpp)
# These two build against the top-level shared main() and its generic main window.
add_qtc_example(Example_Ribbon_ColorIcon qxribbon qxribbon/coloricon shared/main.cpp)
add_qtc_example(Example_RibbonQuickAccessBar qxribbon qxribbon/ribbonquickaccessbar shared/main.cpp)

# -------------------------------------------------------------------- qxdock --
add_qtc_example(Example_Dock_AutoHide qxdock qxdock/autohide qxdock/shared/main.cpp)
add_qtc_example(Example_Dock_CentralWidget qxdock qxdock/centralwidget qxdock/shared/main.cpp)
add_qtc_example(Example_Dock_DockInDock qxdock qxdock/dockindock qxdock/shared/main.cpp)
add_qtc_example(Example_Dock_HideShow qxdock qxdock/hideshow qxdock/shared/main.cpp)
add_qtc_example(Example_Dock_SideBar qxdock qxdock/sidebar qxdock/shared/main.cpp)
add_qtc_example(Example_Dock_ConfigFlags qxdock qxdock/configflags shared/main.cpp)
add_qtc_example(Example_Dock_EmptyDockArea qxdock qxdock/emptydockarea shared/main.cpp)
add_qtc_example(Example_Dock_WidgetToPanel qxdock qxdock/widgettopanel shared/main.cpp)
# The one sample that carries its own main(): nothing shared to add.
add_qtc_example(Example_Dock_DeleteOnClose qxdock qxdock/deleteonclose none)

# --------------------------------------------------------------------- qxapp --
add_qtc_example(Example_RibbonAppEmpty qxapp qxapp/ribbonappempty qxapp/shared/main.cpp)
