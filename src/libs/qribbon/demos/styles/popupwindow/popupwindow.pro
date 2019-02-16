TEMPLATE = app

QTITANDIR = $$(QTITANDIR)

include($$QTITANDIR/src/shared/qtitanribbon.pri)

DESTDIR = $$QTITANDIR/bin
DESTDIR = $$member(DESTDIR, 0)$$QTITAN_LIB_PREFIX
DESTDIR = "$$DESTDIR"

!debug_and_release|build_pass {
    CONFIG(debug, debug|release) {
        TARGET = $$member(TARGET, 0)d
    }
}

HEADERS        = popupwindow.h

SOURCES        = main.cpp \
                 popupwindow.cpp

FORMS         = popupwindowdlg.ui \
                popupwindow.ui

RESOURCES     = popupwindow.qrc


include($$QTITANDIR/demos/shared/aboutdialog.pri)

macx {
    CONFIG-=app_bundle
}

!lessThan(QT_VER_MAJ, 5) {
  QT += widgets
}
