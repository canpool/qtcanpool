##
## Copyright (C) 2026 maminjie <canpool@163.com>
## SPDX-License-Identifier: MulanPSL-2.0
##

# Level 2 module: it requires a Level 1 module, so the manager has to resolve
# output first. PLUGIN_DEPENDS writes that edge into the generated metadata as
# well as into the build, so the two cannot drift apart.
add_qtc_plugin(filetree
  PLUGIN_CLASS FileTreePlugin
  DEPENDS qxplugin Qt5::Core Qt5::Gui Qt5::Widgets
  PLUGIN_DEPENDS output
  SOURCES
    filetreeplugin.cpp filetreeplugin.h
)
