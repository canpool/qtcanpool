##
## Copyright (C) 2026 maminjie <canpool@163.com>
## SPDX-License-Identifier: MulanPSL-2.0
##

# qxapp is here for registerBundledThemes() only: it is the one function that
# knows which theme files res/themes.qrc ships and which of each pair is the
# dark one, so a demo that wants the bundle says so in one call instead of
# carrying a second copy of that table. The rest of the demo is qxdock alone -
# it uses no ribbon and no shell class from qxapp.
add_qtc_executable(DockDemo
  DEPENDS qxdock qxapp qxtheme Qt5::Core Qt5::Widgets
  SOURCES
    main.cpp
    mainwindow.cpp mainwindow.h
    res.qrc
    # The dock style sheet this demo installs in mainwindow.cpp; the library
    # stopped shipping one. res/dockstyle.qrc is shared with any other application
    # that wants the same dock look, the way res/themes.qrc is for the ribbon.
    ${CMAKE_SOURCE_DIR}/res/dockstyle.qrc
    # The style sheets the themes in the Theme menu are read from
    # (:/themes/*.css) - the same bundle ribbondemo offers.
    ${CMAKE_SOURCE_DIR}/res/themes.qrc
)