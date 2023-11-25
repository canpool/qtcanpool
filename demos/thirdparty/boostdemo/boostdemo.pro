TOPDIR = ../../..
include($$TOPDIR/qtproject.pri)
include($$TOPDIR/thirdparty/boost/boost.pri)

QT += core
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = boostdemo
TEMPLATE = app
DESTDIR = $$IDE_APP_PATH

DEFINES += QT_DEPRECATED_WARNINGS

# two ways to link
LIBS += $$boostLibraryName(libboost_system) \
        -l$$boostLibraryName(boost_date_time)

win32 {
    LIBS += -lWs2_32
}

SOURCES += main.cpp
