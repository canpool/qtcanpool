add_qtc_library(qxribbon
  VERSION 0.12.2
  DEFINES QX_RIBBON_LIBRARY
  PUBLIC_DEPENDS Qt5::Core Qt5::Gui Qt5::Widgets
  SOURCES
    qxribbon_global.h
    res.qrc
    ribbonactionsmanager.cpp ribbonactionsmanager.h ribbonactionsmanager_p.h
    ribbonapplicationbutton.cpp ribbonapplicationbutton.h
    ribbonbar.cpp ribbonbar.h ribbonbar_p.h
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
    ribbongallery.cpp ribbongallery.h ribbongallery_p.h
    ribbongallerygroup.cpp ribbongallerygroup.h ribbongallerygroup_p.h
    ribbongroup.cpp ribbongroup.h ribbongroup_p.h
    ribbongrouplayout.cpp ribbongrouplayout.h
    ribbonmainwindow.cpp ribbonmainwindow.h
    ribbonmanager.cpp ribbonmanager.h
    ribbonmenu.cpp ribbonmenu.h
    ribbonpage.cpp ribbonpage.h ribbonpage_p.h
    ribbonpagecontext.cpp ribbonpagecontext.h
    ribbonquickaccessbar.cpp ribbonquickaccessbar.h ribbonquickaccessbar_p.h
    ribbonstyleoption.cpp ribbonstyleoption.h
    ribbontabbar.cpp ribbontabbar.h
    ribbontheme.cpp ribbontheme.h
    ribbonutils.cpp ribbonutils.h
    windowbuttongroup.cpp windowbuttongroup.h windowbuttongroup_p.h
)

