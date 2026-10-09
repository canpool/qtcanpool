add_qtc_library(qxapp
  VERSION 0.0.1
  DEFINES QX_APP_LIBRARY
  DEPENDS qxwindow
  PUBLIC_DEPENDS qxribbon qxdock qxtheme qxcore Qt5::Core Qt5::Gui Qt5::Widgets
  SOURCES
    qxapp_global.h
    qxappshell.cpp qxappshell.h
    qxnavigationbar.cpp qxnavigationbar.h
    qxsplashscreen.cpp qxsplashscreen.h
    ribbonappwindow.cpp ribbonappwindow.h
)
