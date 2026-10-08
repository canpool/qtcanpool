add_qtc_library(qxapp
  VERSION 0.0.1
  DEFINES QX_APP_LIBRARY
  DEPENDS qxwindow
  PUBLIC_DEPENDS qxribbon Qt5::Core Qt5::Gui Qt5::Widgets
  SOURCES
    qxapp_global.h
    ribbonappwindow.cpp ribbonappwindow.h
)
