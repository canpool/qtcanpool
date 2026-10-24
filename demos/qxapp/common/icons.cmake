##
## Copyright (C) 2026 maminjie <canpool@163.com>
## SPDX-License-Identifier: MulanPSL-2.0
##

# The one copy of the demo glyphs: the svgs, the bundle that packs them, and the
# accessor a demo asks for a name through (see icons/icons.h).
#
# A file to include rather than a library: what is shared here is an asset and
# twenty lines of painting, and a library of its own would want a version, an
# install rule and a place in the dependency graph to carry the same twenty
# lines. Each demo keeps compiling the bundle into its own binary, which is what
# all three did before - what stopped being triplicated is the files under svg/.
#
# Asked for as: include("${CMAKE_CURRENT_SOURCE_DIR}/../common/icons.cmake"),
# then ${QXAPP_ICON_SOURCES} in SOURCES and ${QXAPP_ICON_INCLUDES} in INCLUDES.
set(QXAPP_ICON_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/icons/icons.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/icons/icons.h"
    "${CMAKE_CURRENT_LIST_DIR}/icons/icons.qrc"
)
set(QXAPP_ICON_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/icons")
