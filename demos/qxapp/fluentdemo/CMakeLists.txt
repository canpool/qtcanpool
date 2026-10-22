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
add_qtc_executable(FluentDemo
  DEPENDS qxfluent qxapp qxcore qxtheme Qt5::Core Qt5::Widgets
  SOURCES
    main.cpp
    mainwindow.cpp mainwindow.h
    demopage.cpp demopage.h
    homepage.cpp homepage.h
    inputspage.cpp inputspage.h
    feedbackpage.cpp feedbackpage.h
    collectionpage.cpp collectionpage.h
    settingspage.cpp settingspage.h
    # The style sheets the themes the command bar offers are read from
    # (:/themes/*.css) - the same bundle the other demos offer.
    ${CMAKE_SOURCE_DIR}/res/themes.qrc
)

if (TARGET generate_qm_files)
  add_dependencies(FluentDemo generate_qm_files)
endif()
