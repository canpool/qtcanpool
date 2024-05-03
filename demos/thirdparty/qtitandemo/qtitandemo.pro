TOPDIR = ../../..
include($$TOPDIR/qtproject.pri)
include($$TOPDIR/thirdparty/qtitan/qtitan.pri)

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RibbonDockDemo
TEMPLATE = app
DESTDIR = $$IDE_APP_PATH

DEFINES += QT_DEPRECATED_WARNINGS

LIBS += -l$$QtitanLibraryName(qtitan)

HEADERS += \
    $$PWD/ribbondockwindow.h

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/ribbondockwindow.cpp

macx {
    CONFIG-=app_bundle
}

!lessThan(QT_VER_MAJ, 5) {
  QT += printsupport
}
