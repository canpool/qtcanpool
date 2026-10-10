##
## Copyright (C) 2026 maminjie <canpool@163.com>
## SPDX-License-Identifier: MulanPSL-2.0
##

# The IDE-style shell (plan 4.0, task D1): an application assembled out of
# plugins it does not know.
#
# Look at what is missing from DEPENDS - no output, no notebook, no filetree.
# The modules are found in the plugin directory at run time, through their
# metadata, and that absence is the claim this demo exists to make: a link
# against a module would compile the claim away. The whole stack below is here
# because the shell itself uses it (ribbon, docks, theme, window), not because a
# module does.

# Not built in a static configuration (WASM, or QTC_STATIC_BUILD=ON): there a
# plugin is a static library that only the application can pull in with
# Q_IMPORT_PLUGIN, and this application's whole design is that it names no
# module. A shell that discovers plugins in a directory has nothing to discover
# in a build with no directory and no import list, so shipping one would only
# demonstrate the failure. See doc/design/4.0-TASKS.md, D1, for what WASM
# support would cost and why 4.0 does not pay it.
if (QTC_STATIC_BUILD)
  return()
endif()

add_qtc_executable(IdeShellDemo
  # add_qtc_executable defaults to IDE_LIBEXEC_PATH, which is right for a helper
  # tool and wrong for an application: RELATIVE_PLUGIN_PATH - the name that tells
  # the shell where its plugins are - is measured from IDE_BIN_PATH. Leave this
  # line out and the shell looks for its plugins one directory level short of
  # where they are, finds nothing, and still starts (which is the point of the
  # failure isolation, and why only a test catches it). Fix the destination, not
  # the path: on Linux IDE_LIBEXEC_PATH is libexec/qtproject against bin, and on
  # macOS it is <name>.app/Contents/Resources/libexec against <name>.app/Contents/MacOS.
  DESTINATION "${IDE_BIN_PATH}"
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
    shellwindow.cpp shellwindow.h
)
