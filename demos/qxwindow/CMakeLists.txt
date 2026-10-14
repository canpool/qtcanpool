##
## Copyright (C) 2026 maminjie <canpool@163.com>
## SPDX-License-Identifier: MulanPSL-2.0
##

# The demo qxwindow never had: a window with no system frame, a title bar of its
# own, and all four system buttons handed to the agent.
#
# It links qxwindow and nothing else. It does not link qxribbon on purpose - the
# title bar here is a QFrame with a menu bar and four QToolButtons, and a demo
# that borrowed the ribbon's bar would demonstrate the ribbon instead of the
# agent. Nothing else in the tree draws its own bar: RibbonAppWindow brings the
# ribbon's, and every other entry point opens a RibbonMainWindow or a QxAppShell.
add_qtc_executable(QxWindowDemo
  DEPENDS
    qxwindow
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
  SOURCES
    main.cpp
    mainwindow.cpp mainwindow.h
    titlebutton.cpp titlebutton.h
    res.qrc
)
