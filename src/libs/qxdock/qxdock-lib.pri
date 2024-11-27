VERSION = 0.1.0

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += QX_DOCK_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES -= QT_NO_JAVA_STYLE_ITERATORS QT_RESTRICTED_CAST_FROM_ASCII

INCLUDEPATH += $$PWD

PUBLIC_HEADERS = \
    $$PWD/qxdock_global.h \
    $$PWD/dockcontainer.h \
    $$PWD/dockfocuscontroller.h \
    $$PWD/dockpanel.h \
    $$PWD/dockwidget.h \
    $$PWD/dockwindow.h \
    $$PWD/docksplitter.h \
    $$PWD/dockoverlay.h \
    $$PWD/docktitlebar.h \
    $$PWD/docktabbar.h \
    $$PWD/docktab.h \
    $$PWD/dockutils.h \
    $$PWD/dockmanager.h \
    $$PWD/dockiconprovider.h \
    $$PWD/docklabel.h \
    $$PWD/dockfloatingwidget.h \
    $$PWD/dockfloatingcontainer.h \
    $$PWD/dockfloatingpreview.h \
    $$PWD/dockbutton.h \
    $$PWD/docksidebar.h \
    $$PWD/docksidetab.h \
    $$PWD/dockautohidecontainer.h \
    $$PWD/dockresizehandle.h \
    $$PWD/dockgroup.h \
    $$PWD/dockgroupmanager.h \
    $$PWD/docksubwindow.h

PRIVATE_HEADERS = \
    $$PWD/docktitlebar_p.h \ \
    $$PWD/dockgroupmanager_p.h
    $$PWD/dockstatereader.h

HEADERS += \
    $$PUBLIC_HEADERS \
    $$PRIVATE_HEADERS

SOURCES += \
    $$PWD/dockcontainer.cpp \
    $$PWD/dockfocuscontroller.cpp \
    $$PWD/dockiconprovider.cpp \
    $$PWD/dockmanager.cpp \
    $$PWD/dockpanel.cpp \
    $$PWD/dockutils.cpp \
    $$PWD/dockwidget.cpp \
    $$PWD/dockwindow.cpp \
    $$PWD/docksplitter.cpp \
    $$PWD/dockoverlay.cpp \
    $$PWD/docktitlebar.cpp \
    $$PWD/docktabbar.cpp \
    $$PWD/docktab.cpp \
    $$PWD/docklabel.cpp \
    $$PWD/dockfloatingwidget.cpp \
    $$PWD/dockfloatingcontainer.cpp \
    $$PWD/dockfloatingpreview.cpp \
    $$PWD/dockbutton.cpp \
    $$PWD/docksidebar.cpp \
    $$PWD/docksidetab.cpp \
    $$PWD/dockautohidecontainer.cpp \
    $$PWD/dockresizehandle.cpp \
    $$PWD/dockstatereader.cpp \
    $$PWD/dockgroup.cpp \
    $$PWD/dockgroupmanager.cpp \
    $$PWD/docksubwindow.cpp

RESOURCES += \
    $$PWD/res.qrc

unix:!macx {
    HEADERS += $$PWD/linux/dockfloatingtitlebar.h
    SOURCES += $$PWD/linux/dockfloatingtitlebar.cpp
    LIBS += -lxcb
    QT += gui-private
}
