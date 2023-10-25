include(../../qtproject.pri)

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
DESTDIR = $$IDE_APP_PATH

include($$QTCANPOOL_DIR/src/rpath.pri)

INCLUDEPATH += $$PWD

SOURCES += \
    $$PWD/main.cpp

HEADERS += \
    $$PWD/mainwindow.h

RESOURCES += \
    $$PWD/res.qrc
