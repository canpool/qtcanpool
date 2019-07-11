#-------------------------------------------------
#
# Project created by QtCreator 2017-10-22T21:14:13
#
#-------------------------------------------------
include(../../qtproject.pri)

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
TARGET = fancydemo
DESTDIR = $$IDE_APP_PATH

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include($$QTCANPOOL_ROOT/src/rpath.pri)

# lib
LIBS *= -l$$qtLibraryName(qcanpool)

INCLUDEPATH += ..

include(fancydemo-src.pri)
include($$QTCANPOOL_ROOT/src/modules/modules.pri)

win32{
    DEFINES	+= WIN32
    LIBS        += -lshell32
}

CONFIG(release, debug|release){
    DEFINES     += USE_LOG
}


RESOURCES += \
    fancydemo.qrc

RC_FILE = fancydemo.rc

