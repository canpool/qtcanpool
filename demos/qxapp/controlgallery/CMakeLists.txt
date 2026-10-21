##
## Copyright (C) 2026 maminjie <canpool@163.com>
## SPDX-License-Identifier: MulanPSL-2.0
##

# qxapp is here for registerBundledThemes() only: it is the one function that
# knows which theme files res/themes.qrc ships and which of each pair is the
# dark one, so a demo that wants the bundle says so in one call instead of
# carrying a second copy of that table. The two control sets are the whole
# content of the window; qxcontrols and qxcore come with them.
add_qtc_executable(ControlGallery
  DEPENDS qxfluent qxmaterial qxapp qxtheme Qt5::Core Qt5::Widgets
  SOURCES
    main.cpp
    mainwindow.cpp mainwindow.h
    gallerypage.cpp gallerypage.h
    fluentpage.cpp fluentpage.h
    materialpage.cpp materialpage.h
    # The style sheets the themes in the Theme menu are read from
    # (:/themes/*.css) - the same bundle the other demos offer.
    ${CMAKE_SOURCE_DIR}/res/themes.qrc
)
