VERSION = 0.1.0

QT += core gui widgets

win32:equals(QT_MAJOR_VERSION, 5){
    QT += winextras
}

CONFIG += c++11

DEFINES += QGOOD_LIBRARY

# remove some macros in qtproject.pri
DEFINES -= \
    QT_NO_JAVA_STYLE_ITERATORS \
    QT_NO_CAST_TO_ASCII \
    QT_RESTRICTED_CAST_FROM_ASCII \
    "QT_DISABLE_DEPRECATED_BEFORE=0x050900"

SOURCES += \
    $$PWD/qgoodwindow.cpp

HEADERS += \
    $$PWD/qgood_global.h \
    $$PWD/qgoodwindow_p.h \
    $$PWD/qgoodwindow.h

INCLUDEPATH += $$PWD

equals(QT_MAJOR_VERSION, 5){
    DEFINES += QT_VERSION_QT5
}
equals(QT_MAJOR_VERSION, 6) {
    DEFINES += QT_VERSION_QT6
}

win32 { # Windows
    LIBS += -lUser32 -lGdi32
} # Windows

unix:!mac:!android { # Linux
    QT += testlib

    equals(QT_MAJOR_VERSION, 5){
        QT += x11extras
    }

    equals(QT_MAJOR_VERSION, 6){
        QT += gui-private
    }

    QMAKE_CXXFLAGS += -Wno-deprecated-declarations

    LIBS += -lX11

    CONFIG += link_pkgconfig

    equals(QT_MAJOR_VERSION, 5){
        PKGCONFIG += gtk+-2.0
    }
    equals(QT_MAJOR_VERSION, 6){
        PKGCONFIG += gtk+-3.0
    }
} # Linux

mac { # macOS
    OBJECTIVE_SOURCES += \
        $$PWD/qgoodwindow_mac.mm

    LIBS += -framework Foundation -framework Cocoa -framework AppKit
} # macOS
