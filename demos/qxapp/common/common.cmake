##
## Copyright (C) 2026 maminjie <canpool@163.com>
## SPDX-License-Identifier: MulanPSL-2.0
##

# The demo chrome the three applications share, in one place: the glyph set
# (see icons/icons.h), the way a page arrives (see pagetransition.h), and the
# wait before a page has anything to show (see loadingstate.h).
#
# A file to include rather than a library: what is shared here is an asset and
# three small classes, and a library of its own would want a version, an install
# rule and a place in the dependency graph to carry them. Each demo keeps
# compiling the bundle into its own binary, which is what all three did before -
# what stopped being triplicated is the files and the behaviour.
#
# Asked for as: include("${CMAKE_CURRENT_SOURCE_DIR}/../common/common.cmake"),
# then ${QXAPP_COMMON_INCLUDES} in INCLUDES and, in SOURCES, the glyphs alone
# for a demo that never changes page, or the whole list for one that does.
set(QXAPP_COMMON_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(QXAPP_ICON_DIR "${QXAPP_COMMON_DIR}/icons")

# The glyph set, bundled into the binary like the style sheets are.
set(QXAPP_ICON_SOURCES
    "${QXAPP_ICON_DIR}/icons.cpp"
    "${QXAPP_ICON_DIR}/icons.h"
    "${QXAPP_ICON_DIR}/icons.qrc"
)

# Everything the demos with screens share: the two screens of arrival, and the
# glyphs they sit beside.
set(QXAPP_COMMON_SOURCES
    ${QXAPP_ICON_SOURCES}
    "${QXAPP_COMMON_DIR}/pagetransition.cpp"
    "${QXAPP_COMMON_DIR}/pagetransition.h"
    "${QXAPP_COMMON_DIR}/loadingstate.cpp"
    "${QXAPP_COMMON_DIR}/loadingstate.h"
)

set(QXAPP_COMMON_INCLUDES
    "${QXAPP_COMMON_DIR}"
    "${QXAPP_ICON_DIR}"
)
