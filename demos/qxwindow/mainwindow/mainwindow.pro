QTC_LIB_DEPENDS += qxwindow
include(../../../qtproject.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
TARGET = QxWindow_MainWindow
DESTDIR = $$IDE_APP_PATH

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/mainwindow.cpp

HEADERS += \
    $$PWD/mainwindow.h

include($$PWD/../shared/shared.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    $$PWD/mainwindow.qrc
