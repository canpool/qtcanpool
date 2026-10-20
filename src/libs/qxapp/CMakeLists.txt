add_qtc_library(qxapp
  VERSION 0.2.0
  DEFINES QX_APP_LIBRARY
  DEPENDS qxwindow
  PUBLIC_DEPENDS qxribbon qxdock qxtheme qxcore qxplugin Qt5::Core Qt5::Gui Qt5::Widgets
  SOURCES
    appaccessible.cpp
    qxapp_global.h
    qxappshell.cpp qxappshell.h
    qxdockstyle.cpp qxdockstyle.h
    qxextensionbutton.cpp qxextensionbutton.h
    qxmenuaccessbutton.cpp qxmenuaccessbutton.h
    qxmenubutton.cpp qxmenubutton.h
    qxnavtabbar.cpp qxnavtabbar.h qxnavtabbar_p.h
    qxnavigationbar.cpp qxnavigationbar.h
    qxpluginmanagerdialog.cpp qxpluginmanagerdialog.h qxpluginmanagerdialog_p.h
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
    qxthemebundle.cpp qxthemebundle.h
    qxthemeinstaller.cpp qxthemeinstaller.h
    ribbonappwindow.cpp ribbonappwindow.h
)

# See qxribbon's copy of this block for why the catalogue lives here and why
# the target prefixes carry the library name.
if (TARGET Qt5::lrelease)
  add_translation_targets(qxapp
    LANGUAGES zh_CN
    TARGETS qxapp
    OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${IDE_DATA_PATH}/translations"
    INSTALL_DESTINATION "${IDE_DATA_PATH}/translations"
    TS_TARGET_PREFIX "qxapp_ts_"
    QM_TARGET_PREFIX "qxapp_qm_"
  )
endif()
