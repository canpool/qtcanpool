add_qtc_library(qxtheme
  VERSION 0.2.1
  DEFINES QX_THEME_LIBRARY
  PUBLIC_DEPENDS qxcore Qt5::Core Qt5::Gui Qt5::Widgets
  SOURCES
    qxtheme_global.h
    qxtheme.cpp qxtheme.h
    qxthemeelevation.h
    qxthememotion.cpp qxthememotion.h
    qxthememanager.cpp qxthememanager.h
    qxthemepalette.cpp qxthemepalette.h
    qxthemeicon.cpp qxthemeicon.h
)

# The Windows branch of systemAccentColor() reads the DWM key of the registry.
# It is only compiled below Qt 6.6 - from 6.6 on QPalette::Accent carries the
# colour - but the link is unconditional on Windows, because whether it is
# needed is a version the build does not check.
extend_qtc_library(qxtheme CONDITION WIN32
  DEPENDS
    advapi32
)
