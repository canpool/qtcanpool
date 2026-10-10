add_qtc_library(qxapp
  VERSION 0.0.1
  DEFINES QX_APP_LIBRARY
  DEPENDS qxwindow
  PUBLIC_DEPENDS qxribbon qxdock qxtheme qxcore Qt5::Core Qt5::Gui Qt5::Widgets
  SOURCES
    qxapp_global.h
    qxappshell.cpp qxappshell.h
    qxextensionbutton.cpp qxextensionbutton.h
    qxmenuaccessbutton.cpp qxmenuaccessbutton.h
    qxmenubutton.cpp qxmenubutton.h
    qxnavbar.cpp qxnavbar.h qxnavbar_p.h
    qxnavigationbar.cpp qxnavigationbar.h
    qxproperty.h
    qxpropertyeditor.cpp qxpropertyeditor.h qxpropertyeditor_p.h
    qxsettingsdialog.cpp qxsettingsdialog.h qxsettingsdialog_p.h
    qxsplashscreen.cpp qxsplashscreen.h
    qxtabbar.cpp qxtabbar.h qxtabbar_p.h
    qxtabwidget.cpp qxtabwidget.h
    qxtoolbutton.cpp qxtoolbutton.h
    qxtoast.cpp qxtoast.h qxtoast_p.h
    qxtoastmanager.cpp qxtoastmanager.h qxtoastmanager_p.h
    qxworkspacemanager.cpp qxworkspacemanager.h qxworkspacemanager_p.h
    ribbonappwindow.cpp ribbonappwindow.h
)
