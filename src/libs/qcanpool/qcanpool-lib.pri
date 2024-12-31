VERSION = 2.0.2

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += QCANPOOL_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32 {
    DEFINES += QTC_USE_NATIVE
    LIBS += -lUser32
}

PUBLIC_HEADERS = \
    $$PWD/extensionbutton.h \
    $$PWD/fancybar.h \
    $$PWD/fancydialog.h \
    $$PWD/fancytabbar.h \
    $$PWD/fancytabwidget.h \
    $$PWD/fancytitlebar.h \
    $$PWD/fancytoolbutton.h \
    $$PWD/fancywindow.h \
    $$PWD/menuaccessbutton.h \
    $$PWD/menubutton.h \
    $$PWD/minitabbar.h \
    $$PWD/minitabwidget.h \
    $$PWD/qcanpool.h \
    $$PWD/quickaccessbar.h \
    $$PWD/ribbonbar.h \
    $$PWD/ribboncontainers.h \
    $$PWD/ribbongroup.h \
    $$PWD/ribbonpage.h \
    $$PWD/ribbonwindow.h \
    $$PWD/tinynavbar.h \
    $$PWD/tinytabbar.h \
    $$PWD/tinytabwidget.h \
    $$PWD/windowlogo.h \
    $$PWD/windowtoolbar.h

PRIVATE_HEADERS = \
    $$PWD/fancybar_p.h \
    $$PWD/fancytabbar_p.h \
    $$PWD/fancytitlebar_p.h \
    $$PWD/quickaccessbar_p.h \
    $$PWD/ribbonbar_p.h \
    $$PWD/ribbongroup_p.h \
    $$PWD/ribbonpage_p.h \
    $$PWD/tinynavbar_p.h \
    $$PWD/tinytabbar_p.h \
    $$PWD/windowtoolbar_p.h

HEADERS += \
    $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS

SOURCES += \
    $$PWD/extensionbutton.cpp \
    $$PWD/fancybar.cpp \
    $$PWD/fancydialog.cpp \
    $$PWD/fancytabbar.cpp \
    $$PWD/fancytabwidget.cpp \
    $$PWD/fancytitlebar.cpp \
    $$PWD/fancytoolbutton.cpp \
    $$PWD/fancywindow.cpp \
    $$PWD/menuaccessbutton.cpp \
    $$PWD/menubutton.cpp \
    $$PWD/minitabbar.cpp \
    $$PWD/minitabwidget.cpp \
    $$PWD/quickaccessbar.cpp \
    $$PWD/ribbonbar.cpp \
    $$PWD/ribboncontainers.cpp \
    $$PWD/ribbongroup.cpp \
    $$PWD/ribbonpage.cpp \
    $$PWD/ribbonwindow.cpp \
    $$PWD/tinynavbar.cpp \
    $$PWD/tinytabbar.cpp \
    $$PWD/tinytabwidget.cpp \
    $$PWD/windowlogo.cpp \
    $$PWD/windowtoolbar.cpp

RESOURCES += \
    $$PWD/qcanpool.qrc
