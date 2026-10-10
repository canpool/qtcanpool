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
    $$PWD/windowtoolbar_p.h

# extensionbutton.h, fancytoolbutton.h, menuaccessbutton.h, menubutton.h,
# tinynavbar.h, tinytabbar.h and tinytabwidget.h are still listed above, but
# they no longer carry an implementation: the widgets moved to qxapp in 3.1 and
# these headers are deprecated forwarding classes now.
SOURCES += \
    $$PWD/fancybar.cpp \
    $$PWD/fancydialog.cpp \
    $$PWD/fancytabbar.cpp \
    $$PWD/fancytabwidget.cpp \
    $$PWD/fancytitlebar.cpp \
    $$PWD/fancywindow.cpp \
    $$PWD/minitabbar.cpp \
    $$PWD/minitabwidget.cpp \
    $$PWD/quickaccessbar.cpp \
    $$PWD/windowlogo.cpp \
    $$PWD/windowtoolbar.cpp

HEADERS += \
    $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS

RESOURCES += \
    $$PWD/qcanpool.qrc
