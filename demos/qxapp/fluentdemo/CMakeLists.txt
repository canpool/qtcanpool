##
## Copyright (C) 2026 maminjie <canpool@163.com>
## SPDX-License-Identifier: MulanPSL-2.0
##

# A Fluent application, not a gallery: a command bar, a navigation pane, and
# five screens built out of QxFluent alone. Where the control gallery puts one
# control in each row so that a change in what a control paints shows up, this
# puts them to work - which is the other half of what a control set has to
# prove, and the half that says whether an application can be built out of it.
#
# qxapp is here for registerBundledThemes() only: it is the one function that
# knows which theme files res/themes.qrc ships and which of each pair is the
# dark one. qxcore carries QxTranslator, which is how --lang is answered.
# Where the built catalogues are - the same define ribbondemo is given. The two
# control libraries hold no tr() strings of their own yet, so what a language
# buys here is the shared catalogue's strings (qxapp's) rather than the screens',
# but --lang answering with what exists beats warning that nothing is installed.
if (TARGET Qt5::lrelease)
  set(_fluentdemo_defines
    "QTCANPOOL_TRANSLATIONS_DIR=\"${CMAKE_BINARY_DIR}/${IDE_DATA_PATH}/translations\"")
endif()

# The glyphs the pane puts beside its destinations, drawn in the pane's ink:
# the one set the demos share, painted per theme rather than shipped per demo.
include("${CMAKE_CURRENT_SOURCE_DIR}/../common/icons.cmake")

add_qtc_executable(FluentDemo
  DEPENDS qxfluent qxapp qxcore qxtheme qxwindow Qt5::Core Qt5::Widgets
  DEFINES ${_fluentdemo_defines}
  INCLUDES ${QXAPP_ICON_INCLUDES}
  SOURCES
    main.cpp
    mainwindow.cpp mainwindow.h
    titlebar.cpp titlebar.h
    demopage.cpp demopage.h
    controlpages.cpp controlpages.h
    homepage.cpp homepage.h
    settingspage.cpp settingspage.h
    # The shared glyph set, bundled into this binary like the themes below.
    ${QXAPP_ICON_SOURCES}
    # The style sheets the themes the command bar offers are read from
    # (:/themes/*.css) - the same bundle the other demos offer.
    ${CMAKE_SOURCE_DIR}/res/themes.qrc
)

if (TARGET generate_qm_files)
  add_dependencies(FluentDemo generate_qm_files)
endif()
