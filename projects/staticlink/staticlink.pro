QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
DESTDIR = bin

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

LIBSRC_DIR = $$PWD/../../src/libs
INCLUDEPATH += $$LIBSRC_DIR
include($$LIBSRC_DIR/qxwindow/qxwindow.pri)
include($$LIBSRC_DIR/qxdock/qxdock.pri)

# There are conflict ribbon files with the same name in qxribbon and qcanpool
QCANPOOL_CONFIG_RIBBON = 0
equals(QCANPOOL_CONFIG_RIBBON, 1) {
    include($$LIBSRC_DIR/qcanpool/qcanpool.pri)
} else {
    include($$LIBSRC_DIR/qxribbon/qxribbon.pri)
    include($$LIBSRC_DIR/qcanpool/qcanpool.pri)
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
