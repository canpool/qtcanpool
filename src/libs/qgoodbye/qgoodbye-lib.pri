VERSION = 0.1.0

QT += core gui widgets svg

win32:equals(QT_MAJOR_VERSION, 5){
    QT += winextras
}

CONFIG += c++11

DEFINES += QGOODBYE_LIBRARY

# remove some macros in qtproject.pri
DEFINES -= \
    QT_NO_JAVA_STYLE_ITERATORS \
    QT_NO_CAST_TO_ASCII \
    QT_RESTRICTED_CAST_FROM_ASCII \
    "QT_DISABLE_DEPRECATED_BEFORE=0x050900"

SOURCES += \
    $$PWD/qgoodcaptionbutton.cpp \
    $$PWD/qgooddialog.cpp \
    $$PWD/qgoodiconwidget.cpp \
    $$PWD/qgoodmanager.cpp \
    $$PWD/qgoodstyle.cpp \
    $$PWD/qgoodstyleHelper.cpp \
    $$PWD/qgoodtitlebar.cpp \
    $$PWD/qgoodtitlewidget.cpp \
    $$PWD/qgoodstateholder.cpp \
    $$PWD/qgoodcentralwidget.cpp

HEADERS += \
    $$PWD/qgoodbye_global.h \
    $$PWD/qgoodcaptionbutton.h \
    $$PWD/qgooddialog.h \
    $$PWD/qgoodiconwidget.h \
    $$PWD/qgoodmanager.h \
    $$PWD/qgoodstyle.h \
    $$PWD/qgoodstyleHelper.h \
    $$PWD/qgoodtitlebar.h \
    $$PWD/qgoodtitlewidget.h \
    $$PWD/qgoodstateholder.h \
    $$PWD/qgoodcentralwidget.h

RESOURCES += \
    $$PWD/qgoodbye.qrc

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
