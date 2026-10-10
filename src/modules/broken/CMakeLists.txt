##
## Copyright (C) 2026 maminjie <canpool@163.com>
## SPDX-License-Identifier: MulanPSL-2.0
##

# NOT BUILT. src/modules/CMakeLists.txt deliberately does not add this
# directory: the module below declares a dependency that cannot be satisfied,
# so building it into the plugin directory would make every run of the samples
# carry a failure. Keep it as source, for the documentation and screenshots.
#
# To see what the manager does with it, copy this directory next to a real
# module and add the subdirectory there.
add_qtc_plugin(broken
  PLUGIN_CLASS BrokenPlugin
  DEPENDS qxplugin Qt5::Core Qt5::Gui Qt5::Widgets
  PLUGIN_DEPENDS nonexistent
  SOURCES
    brokenplugin.cpp brokenplugin.h
)
