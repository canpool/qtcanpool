##
## Copyright (C) 2026 maminjie <canpool@163.com>
## SPDX-License-Identifier: MulanPSL-2.0
##

# Level 2 module: the tree reports what it opens to the output panel, but it
# works without one, so output is an optional dependency rather than a required
# one. PLUGIN_RECOMMENDS writes that into the generated metadata as
# "Type" : "optional", and the manager reads it from there: output is
# initialized first when it is present, filetree starts anyway when it is not,
# and switching output off does not take the file tree with it.
add_qtc_plugin(filetree
  PLUGIN_CLASS FileTreePlugin
  DEPENDS qxplugin Qt5::Core Qt5::Gui Qt5::Widgets
  PLUGIN_RECOMMENDS output
  SOURCES
    filetreeplugin.cpp filetreeplugin.h
)
