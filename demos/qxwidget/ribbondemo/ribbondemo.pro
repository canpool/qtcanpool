QTC_LIB_DEPENDS += qxribbon

greaterThan(QT_MAJOR_VERSION, 5) {
win32 {
    QTC_LIB_DEPENDS += qxwindow
    DEFINES += QXRIBBON_USE_APPWINDOW
    SOURCES += $$PWD/ribbonappwindow.cpp
    HEADERS += $$PWD/ribbonappwindow.h
}
}

include(../../../qtproject.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = RibbonDemo
TEMPLATE = app
DESTDIR = $$IDE_APP_PATH

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include($$QTCANPOOL_DIR/src/rpath.pri)

SOURCES += \
    $$PWD/aboutdialog.cpp \
    $$PWD/editablecontainer.cpp \
    $$PWD/main.cpp \
    $$PWD/mainwindow.cpp

HEADERS += \
    $$PWD/aboutdialog.h \
    $$PWD/editablecontainer.h \
    $$PWD/mainwindow.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    $$PWD/ribbondemo.qrc

RC_FILE = $$PWD/ribbondemo.rc
