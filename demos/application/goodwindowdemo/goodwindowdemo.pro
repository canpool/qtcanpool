QTC_LIB_DEPENDS += qgood qcanpool
include(../../../qtproject.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = goodwindowdemo
TEMPLATE = app
DESTDIR = $$IDE_APP_PATH

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include($$QTCANPOOL_DIR/src/rpath.pri)

SOURCES += \
    goodmainwindow.cpp \
    main.cpp

HEADERS += \
    goodmainwindow.h

RESOURCES += \
    $$PWD/res.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
