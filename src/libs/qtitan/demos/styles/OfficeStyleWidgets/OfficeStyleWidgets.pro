TEMPLATE = app

TARGET = "Ribbon_"$$member(TARGET, 0)

QTITANDIR = $$quote($$(QTITANDIR))
isEmpty(QTITANDIR):QTITANDIR = $$quote($$PWD/../../../)
include($$QTITANDIR/src/shared/qtitanribbon.pri)

DESTDIR = $$QTITANDIR/bin
DESTDIR = $$member(DESTDIR, 0)$$QTITAN_LIB_PREFIX

!debug_and_release|build_pass {
    CONFIG(debug, debug|release) {
        TARGET = $$member(TARGET, 0)d
    }
}

HEADERS       = mainwindow.h \
                dockwidgets.h

SOURCES       = main.cpp \
                mainwindow.cpp \
                dockwidgets.cpp
                
RESOURCES     = OfficeStyleWidgets.qrc
FORMS         = OfficeStyleDialog.ui

include($$PWD/../../shared/aboutdialog.pri)

macx {
    CONFIG-=app_bundle
}

!lessThan(QT_VER_MAJ, 5) {
  QT += widgets
}
