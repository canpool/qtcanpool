##
## Copyright (C) 2026 maminjie <canpool@163.com>
## SPDX-License-Identifier: MulanPSL-2.0
##

# The AppShell combines the ribbon, the dock system and the theme engine, so the
# demo links the whole stack.
add_qtc_executable(AppShellDemo
  DEPENDS
    qxapp
    qxribbon
    qxdock
    qxwindow
    qxtheme
    qxcore
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
  SOURCES
    main.cpp
    mainwindow.cpp mainwindow.h
)
