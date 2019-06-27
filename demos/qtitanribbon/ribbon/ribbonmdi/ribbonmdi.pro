QTITANDIR = $$quote($$PWD/../..)
include(../../../../qtproject.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
DESTDIR = $$IDE_APP_PATH

LIBS *= -l$$qtLibraryNameVersion(qribbon, 3)

INCLUDEPATH += $$QTCANPOOL_ROOT/src/libs/qribbon/include

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

include($$QTITANDIR/shared/aboutdialog.pri)

macx {
    CONFIG-=app_bundle
}

