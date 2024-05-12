TEMPLATE = app

QTITANDIR = $$quote($$(QTITANDIR))

isEmpty(QTITANDIR):QTITANDIR = $$quote($$PWD/../../../)

include($$QTITANDIR/src/shared/qtitanribbon.pri)

DESTDIR = $$IDE_BUILD_TREE/bin
DESTDIR = $$member(DESTDIR, 0)$$QTITAN_LIB_PREFIX
DESTDIR = "$$DESTDIR"

!debug_and_release|build_pass {
    CONFIG(debug, debug|release) {
        TARGET = $$member(TARGET, 0)d
    }
}

HEADERS        = mainwindow.h \
                 backstagepages.h

SOURCES        = main.cpp \
                 mainwindow.cpp \
                 backstagepages.cpp

FORMS         = infopage.ui \
                savepage.ui \
                helppage.ui \
                saveemailpage.ui \
                savewebpage.ui \

RESOURCES     = RibbonBackstageView.qrc


include($$PWD/../../shared/aboutdialog.pri)
include($$PWD/../../ribbon/shared/ribbonwindow.pri)

macx {
    CONFIG-=app_bundle
}

!lessThan(QT_VER_MAJ, 5) {
  QT += printsupport
}
