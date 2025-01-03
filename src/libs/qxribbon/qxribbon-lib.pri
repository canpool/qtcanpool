VERSION = 0.9.1

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += QX_RIBBON_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

PUBLIC_HEADERS = \
    $$PWD/qxribbon_global.h \
    $$PWD/ribbonactionsmanager.h \
    $$PWD/ribbonapplicationbutton.h \
    $$PWD/ribbonbar.h \
    $$PWD/ribbonbutton.h \
    $$PWD/ribbonbuttongroup.h \
    $$PWD/ribboncolorbutton.h \
    $$PWD/ribboncontainers.h \
    $$PWD/ribboncontrols.h \
    $$PWD/ribboncustomizedialog.h \
    $$PWD/ribbonfactory.h \
    $$PWD/ribbongallery.h \
    $$PWD/ribbongallerygroup.h \
    $$PWD/ribbongroup.h \
    $$PWD/ribbonmanager.h \
    $$PWD/ribbonmenu.h \
    $$PWD/ribbonpage.h \
    $$PWD/ribbonpagecontext.h \
    $$PWD/ribbonquickaccessbar.h \
    $$PWD/ribbonstyleoption.h \
    $$PWD/ribbontheme.h \
    $$PWD/ribbonutils.h \
    $$PWD/ribbonwindow.h \
    $$PWD/windowbuttongroup.h

PRIVATE_HEADERS = \
    $$PWD/framelesshelper.h \
    $$PWD/ribbonactionsmanager_p.h \
    $$PWD/ribbontabbar.h \
    $$PWD/ribbonbar_p.h \
    $$PWD/ribbonbutton_p.h \
    $$PWD/ribboncolorbutton_p.h \
    $$PWD/ribboncustomizedata.h \
    $$PWD/ribboncustomizewidget.h \
    $$PWD/ribboncustomizewidget_p.h \
    $$PWD/ribbongallery_p.h \
    $$PWD/ribbongallerygroup_p.h \
    $$PWD/ribbongroup_p.h \
    $$PWD/ribbongrouplayout.h \
    $$PWD/ribbonpage_p.h \
    $$PWD/ribbonquickaccessbar_p.h \
    $$PWD/windowbuttongroup_p.h

HEADERS += \
    $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS

SOURCES += \
    $$PWD/framelesshelper.cpp \
    $$PWD/ribbonactionsmanager.cpp \
    $$PWD/ribbonapplicationbutton.cpp \
    $$PWD/ribbonbar.cpp \
    $$PWD/ribbonbutton.cpp \
    $$PWD/ribbonbuttongroup.cpp \
    $$PWD/ribboncolorbutton.cpp \
    $$PWD/ribboncontainers.cpp \
    $$PWD/ribboncontrols.cpp \
    $$PWD/ribboncustomizedata.cpp \
    $$PWD/ribboncustomizedialog.cpp \
    $$PWD/ribboncustomizewidget.cpp \
    $$PWD/ribbonfactory.cpp \
    $$PWD/ribbongallery.cpp \
    $$PWD/ribbongallerygroup.cpp \
    $$PWD/ribbongroup.cpp \
    $$PWD/ribbongrouplayout.cpp \
    $$PWD/ribbonmanager.cpp \
    $$PWD/ribbonmenu.cpp \
    $$PWD/ribbonpage.cpp \
    $$PWD/ribbonpagecontext.cpp \
    $$PWD/ribbonquickaccessbar.cpp \
    $$PWD/ribbonstyleoption.cpp \
    $$PWD/ribbontabbar.cpp \
    $$PWD/ribbontheme.cpp \
    $$PWD/ribbonutils.cpp \
    $$PWD/ribbonwindow.cpp \
    $$PWD/windowbuttongroup.cpp

RESOURCES += \
    $$PWD/res.qrc

win32 {
    # for FramelessHelper
    LIBS += -lUser32
    lessThan(QT_MAJOR_VERSION, 6): DEFINES += FRAMELESS_USE_NATIVE
}
