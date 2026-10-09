add_qtc_library(qxtheme
  VERSION 0.1.0
  DEFINES QX_THEME_LIBRARY
  PUBLIC_DEPENDS qxcore Qt5::Core Qt5::Gui Qt5::Widgets
  SOURCES
    qxtheme_global.h
    qxtheme.cpp qxtheme.h
    qxthememanager.cpp qxthememanager.h
    qxthemepalette.cpp qxthemepalette.h
)
