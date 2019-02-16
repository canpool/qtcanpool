TEMPLATE = app

QTITANDIR = $$quote($$(QTITANDIR))

isEmpty(QTITANDIR):QTITANDIR = $$quote($$PWD/../../..)

include($$QTITANDIR/src/shared/qtitanribbon.pri)

#DESTDIR = $$QTITANDIR/bin
DESTDIR = $$IDE_APP_PATH
DESTDIR = $$member(DESTDIR, 0)$$QTITAN_LIB_PREFIX

# auto copy dll to STATIC_OUTPUT_BASE
STATIC_OUTPUT_BASE = $$DESTDIR
include($$QTITANDIR/../../data.pri)

!debug_and_release|build_pass {
    CONFIG(debug, debug|release) {
        TARGET = $$member(TARGET, 0)d
    }
}

HEADERS       = mainwindow.h \
                texteditchild.h \
                sketchchild.h \
                sketchdocument.h \
                sketchobject.h \
                sketchtool.h \
                sketchrepresentation.h \
                childinterface.h

SOURCES       = main.cpp \
                mainwindow.cpp \
                texteditchild.cpp \
                sketchchild.cpp \
                sketchdocument.cpp \
                sketchobject.cpp \
                sketchtool.cpp \
                sketchrepresentation.cpp

RESOURCES     = ribbonmdi.qrc

include($$QTITANDIR/demos/shared/aboutdialog.pri)

macx {
    CONFIG-=app_bundle
}

