VERSION = 1.0.0

QT += xml

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets core-private widgets-private gui-private
}
# remove some macros in qtproject.pri
DEFINES -= \
    QT_NO_JAVA_STYLE_ITERATORS \
    QT_NO_CAST_TO_ASCII \
    QT_RESTRICTED_CAST_FROM_ASCII \
    "QT_DISABLE_DEPRECATED_BEFORE=0x050900"

DEFINES += QRIBBON_LIBRARY
DEFINES += QRIBBON_VERSION=$$VERSION

HEADERS += \
    $$PWD/commonstyle.h \
    $$PWD/commonstyle_p.h \
    $$PWD/officeframehelper.h \
    $$PWD/officepopupcolorbutton.h \
    $$PWD/officepopupmenu.h \
    $$PWD/officepopupwindow.h \
    $$PWD/officestyle.h \
    $$PWD/officestyle_p.h \
    $$PWD/popuphelpers.h \
    $$PWD/ribbon.h \
    $$PWD/ribbon_def.h \
    $$PWD/ribbon_global.h \
    $$PWD/ribbon_p.h \
    $$PWD/ribbonbackstageview.h \
    $$PWD/ribbonbackstageview_p.h \
    $$PWD/ribbonbar.h \
    $$PWD/ribbonbar_p.h \
    $$PWD/ribbonbutton_p.h \
    $$PWD/ribbonbuttoncontrols.h \
    $$PWD/ribboncontrols.h \
    $$PWD/ribboncontrols_p.h \
    $$PWD/ribboncustomizedialog.h \
    $$PWD/ribboncustomizedialog_p.h \
    $$PWD/ribboncustomizemanager.h \
    $$PWD/ribboncustomizemanager_p.h \
    $$PWD/ribboncustomizepage.h \
    $$PWD/ribbongallery.h \
    $$PWD/ribbongallery_p.h \
    $$PWD/ribbongallerycontrols.h \
    $$PWD/ribbongroup.h \
    $$PWD/ribbongroup_p.h \
    $$PWD/ribboninputcontrols.h \
    $$PWD/ribbonmainwindow.h \
    $$PWD/ribbonpage.h \
    $$PWD/ribbonpage_p.h \
    $$PWD/ribbonquickaccessbar.h \
    $$PWD/ribbonsliderpane.h \
    $$PWD/ribbonstatusbar.h \
    $$PWD/ribbonstyle.h \
    $$PWD/ribbonstyle_p.h \
    $$PWD/ribbonsystempopupbar.h \
    $$PWD/ribbontabbar.h \
    $$PWD/ribbontooltip.h \
    $$PWD/ribbonworkspace.h \
    $$PWD/stylehelpers.h

SOURCES += \
    $$PWD/commonstyle.cpp \
    $$PWD/officepaintmanager.cpp \
    $$PWD/officepopupcolorbutton.cpp \
    $$PWD/officepopupmenu.cpp \
    $$PWD/officepopupwindow.cpp \
    $$PWD/officestyle.cpp \
    $$PWD/popuphelpers.cpp \
    $$PWD/ribbon_def.cpp \
    $$PWD/ribbon_p.cpp \
    $$PWD/ribbonbackstageview.cpp \
    $$PWD/ribbonbar.cpp \
    $$PWD/ribbonbutton_p.cpp \
    $$PWD/ribbonbuttoncontrols.cpp \
    $$PWD/ribboncontrols.cpp \
    $$PWD/ribboncustomizedialog.cpp \
    $$PWD/ribboncustomizemanager.cpp \
    $$PWD/ribboncustomizepage.cpp \
    $$PWD/ribbongallery.cpp \
    $$PWD/ribbongallerycontrols.cpp \
    $$PWD/ribbongroup.cpp \
    $$PWD/ribboninputcontrols.cpp \
    $$PWD/ribbonmainwindow.cpp \
    $$PWD/ribbonpage.cpp \
    $$PWD/ribbonpaintmanager.cpp \
    $$PWD/ribbonquickaccessbar.cpp \
    $$PWD/ribbonsliderpane.cpp \
    $$PWD/ribbonstatusbar.cpp \
    $$PWD/ribbonstyle.cpp \
    $$PWD/ribbonsystempopupbar.cpp \
    $$PWD/ribbontabbar.cpp \
    $$PWD/ribbontooltip.cpp \
    $$PWD/ribbonworkspace.cpp \
    $$PWD/stylehelpers.cpp

win32 {
    HEADERS += \
        $$PWD/officeframehelper_win.h \

    SOURCES += \
        $$PWD/commonstyle_win.cpp \
        $$PWD/officeframehelper_win.cpp \

    DEFINES += _WINDOWS
    LIBS += -luser32 -lgdi32 -lshell32 -lcomctl32
}

linux* {
    SOURCES += \
        $$PWD/commonstyle_linux.cpp
}

macx {
    SOURCES += \
        $$PWD/commonstyle_mac.mm

    LIBS += -framework AppKit
}

FORMS += \
    $$PWD/ribbonbarcustomizepage.ui \
    $$PWD/ribbonquickaccessbarcustomizepage.ui \
    $$PWD/ribbonrenamedialog.ui


RESOURCES += \
    $$PWD/ribbon.qrc
