##
## Copyright (C) 2026 maminjie <canpool@163.com>
## SPDX-License-Identifier: MulanPSL-2.0
##

# Level 1 module: a page in the central workspace plus its rail entry.
add_qtc_plugin(notebook
  PLUGIN_CLASS NotebookPlugin
  DEPENDS qxplugin Qt5::Core Qt5::Gui Qt5::Widgets
  SOURCES
    notebookplugin.cpp notebookplugin.h
)
