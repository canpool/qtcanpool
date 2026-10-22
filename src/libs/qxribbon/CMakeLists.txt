add_qtc_library(qxribbon
  VERSION 0.14.1
  DEFINES QX_RIBBON_LIBRARY
  PUBLIC_DEPENDS Qt5::Core Qt5::Gui Qt5::Widgets
  SOURCES
    qxribbon_global.h
    res.qrc
    ribbonaccessible.cpp
    ribbonactionsmanager.cpp ribbonactionsmanager.h ribbonactionsmanager_p.h
    ribbonapplicationbutton.cpp ribbonapplicationbutton.h
    ribbonbackstageview.cpp ribbonbackstageview.h ribbonbackstageview_p.h
    ribbonbar.cpp ribbonbar.h ribbonbar_p.h
    ribbonbarlayout.cpp
    ribbonbarpaint.cpp
    ribbonbarprivate.cpp
    ribbonbarstackedwidget.cpp
    ribbonbutton.cpp ribbonbutton.h ribbonbutton_p.h
    ribbonbuttongroup.cpp ribbonbuttongroup.h
    ribboncolorbutton.cpp ribboncolorbutton.h ribboncolorbutton_p.h
    ribboncontainers.cpp ribboncontainers.h
    ribboncontrols.cpp ribboncontrols.h
    ribboncustomizedata.cpp ribboncustomizedata.h
    ribboncustomizedialog.cpp ribboncustomizedialog.h
    ribboncustomizewidget.cpp ribboncustomizewidget.h ribboncustomizewidget_p.h
    ribbonfactory.cpp ribbonfactory.h
    ribbonkeytip.cpp ribbonkeytip.h
    ribbonscreentip.cpp ribbonscreentip.h
    ribbongallery.cpp ribbongallery.h ribbongallery_p.h
    ribbongallerygroup.cpp ribbongallerygroup.h ribbongallerygroup_p.h
    ribbongroup.cpp ribbongroup.h ribbongroup_p.h
    ribbongrouplayout.cpp ribbongrouplayout.h
    ribbonmainwindow.cpp ribbonmainwindow.h
    ribbonmdiarea.cpp ribbonmdiarea.h
    ribbonmanager.cpp ribbonmanager.h
    ribbonmenu.cpp ribbonmenu.h
    ribbonpage.cpp ribbonpage.h ribbonpage_p.h
    ribbonpagecontext.cpp ribbonpagecontext.h
    ribbonquickaccessbar.cpp ribbonquickaccessbar.h ribbonquickaccessbar_p.h
    ribbonsliderpane.cpp ribbonsliderpane.h ribbonsliderpane_p.h
    ribbonstatusbar.cpp ribbonstatusbar.h ribbonstatusbar_p.h
    ribbonstyleoption.cpp ribbonstyleoption.h
    ribbontabbar.cpp ribbontabbar.h
    ribbonutils.cpp ribbonutils.h
    windowbuttongroup.cpp windowbuttongroup.h windowbuttongroup_p.h
)

# Translations. A library owns the strings that its own widgets show, so it
# carries the catalogue for them beside its sources and updating one library's
# strings never touches another's catalogue. The generated catalogues all land
# in one shared directory because QxTranslator reads a single path, and that
# directory repeats the install one under CMAKE_BINARY_DIR so a build tree and
# an installed tree answer to the same lookup.
#
# The tool targets have to carry the library name: add_custom_target() names
# are global, and three libraries calling this function would otherwise all
# claim the same "ts_zh_CN". Only the umbrella target is shared, and it is the
# one the function creates only once.
if (TARGET Qt5::lrelease)
  add_translation_targets(qxribbon
    LANGUAGES zh_CN
    TARGETS qxribbon
    OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${IDE_DATA_PATH}/translations"
    INSTALL_DESTINATION "${IDE_DATA_PATH}/translations"
    TS_TARGET_PREFIX "qxribbon_ts_"
    QM_TARGET_PREFIX "qxribbon_qm_"
  )
endif()

