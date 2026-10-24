##
## Copyright (C) 2026 maminjie <canpool@163.com>
## SPDX-License-Identifier: MulanPSL-2.0
##

# A Material application, not a gallery: a top app bar, a navigation rail, a
# floating action button, a snackbar, six template screens built out of
# QxMaterial alone, and the catalogue of what the library ships. Where the
# control gallery puts one control in each row so that a change in what a
# control paints shows up, this puts them to work - which is the other half of
# what a control set has to prove, and the half that says whether an
# application can be built out of it.
#
# qxapp is here for registerBundledThemes() only: it is the one function that
# knows which theme files res/themes.qrc ships and which of each pair is the
# dark one. qxcore carries QxTranslator, which is how --lang is answered.
# qxwindow is what takes the system frame away and makes the app bar the
# caption; the bar itself is still the one qxmaterial ships.
# Where the built catalogues are - the same define ribbondemo is given. The two
# control libraries hold no tr() strings of their own yet, so what a language
# buys here is the shared catalogue's strings (qxapp's) rather than the screens',
# but --lang answering with what exists beats warning that nothing is installed.
if (TARGET Qt5::lrelease)
  set(_materialdemo_defines
    "QTCANPOOL_TRANSLATIONS_DIR=\"${CMAKE_BINARY_DIR}/${IDE_DATA_PATH}/translations\"")
endif()

# The glyphs the bar and the rail draw, and the transition a screen arrives
# over: the chrome the demos share rather than one copy each.
include("${CMAKE_CURRENT_SOURCE_DIR}/../common/common.cmake")

add_qtc_executable(MaterialDemo
  DEPENDS qxmaterial qxapp qxcore qxtheme qxwindow Qt5::Core Qt5::Widgets
  DEFINES ${_materialdemo_defines}
  INCLUDES ${QXAPP_COMMON_INCLUDES}
  SOURCES
    main.cpp
    mainwindow.cpp mainwindow.h
    titlebar.cpp titlebar.h
    demopage.cpp demopage.h
    # The six templates: a whole screen each, built out of the library alone.
    homepage.cpp homepage.h
    albumpage.cpp albumpage.h
    pricingpage.cpp pricingpage.h
    signinpage.cpp signinpage.h
    blogpage.cpp blogpage.h
    orderpage.cpp orderpage.h
    # The library itself, one control per row, and the window's own settings.
    catalogpage.cpp catalogpage.h
    settingspage.cpp settingspage.h
    ${QXAPP_COMMON_SOURCES}
    # The style sheets the themes the settings screen offers are read from
    # (:/themes/*.css) - the same bundle the other demos offer.
    ${CMAKE_SOURCE_DIR}/res/themes.qrc
)

if (TARGET generate_qm_files)
  add_dependencies(MaterialDemo generate_qm_files)
endif()
