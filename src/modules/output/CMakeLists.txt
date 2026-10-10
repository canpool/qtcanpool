##
## Copyright (C) 2026 maminjie <canpool@163.com>
## SPDX-License-Identifier: MulanPSL-2.0
##

# Level 1 module: a dock panel plus a line on the status bar. No dependency on
# another module, which is what lets filetree require it.
add_qtc_plugin(output
  PLUGIN_CLASS OutputPlugin
  DEPENDS qxplugin Qt5::Core Qt5::Gui Qt5::Widgets
  SOURCES
    outputplugin.cpp outputplugin.h
)
