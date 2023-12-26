QTC_LIB_DEPENDS += qxmaterial
include(../../../qtproject.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = Example_MaterialComponent
TEMPLATE = app
DESTDIR = $$IDE_APP_PATH

SOURCES = \
    $$PWD/mainwindow.cpp \
    $$PWD/main.cpp \
    $$PWD/avatarsettingseditor.cpp \
    $$PWD/badgesettingseditor.cpp \
    $$PWD/checkboxsettingseditor.cpp \
    $$PWD/fabsettingseditor.cpp \
    $$PWD/raisedbuttonsettingseditor.cpp \
    $$PWD/flatbuttonsettingseditor.cpp \
    $$PWD/iconbuttonsettingseditor.cpp \
    $$PWD/progresssettingseditor.cpp \
    $$PWD/circularprogresssettingseditor.cpp \
    $$PWD/slidersettingseditor.cpp \
    $$PWD/radiobuttonsettingseditor.cpp \
    $$PWD/togglesettingseditor.cpp \
    $$PWD/textfieldsettingseditor.cpp \
    $$PWD/tabssettingseditor.cpp \
    $$PWD/snackbarsettingseditor.cpp \
    $$PWD/dialogsettingseditor.cpp \
    $$PWD/drawersettingseditor.cpp \
    $$PWD/scrollbarsettingseditor.cpp \
    $$PWD/appbarsettingseditor.cpp \
    $$PWD/autocompletesettingseditor.cpp

HEADERS = \
    $$PWD/mainwindow.h \
    $$PWD/avatarsettingseditor.h \
    $$PWD/badgesettingseditor.h \
    $$PWD/checkboxsettingseditor.h \
    $$PWD/fabsettingseditor.h \
    $$PWD/raisedbuttonsettingseditor.h \
    $$PWD/flatbuttonsettingseditor.h \
    $$PWD/iconbuttonsettingseditor.h \
    $$PWD/progresssettingseditor.h \
    $$PWD/circularprogresssettingseditor.h \
    $$PWD/slidersettingseditor.h \
    $$PWD/radiobuttonsettingseditor.h \
    $$PWD/togglesettingseditor.h \
    $$PWD/textfieldsettingseditor.h \
    $$PWD/tabssettingseditor.h \
    $$PWD/snackbarsettingseditor.h \
    $$PWD/dialogsettingseditor.h \
    $$PWD/drawersettingseditor.h \
    $$PWD/scrollbarsettingseditor.h \
    $$PWD/appbarsettingseditor.h \
    $$PWD/autocompletesettingseditor.h

RESOURCES += \
    $$PWD/examples.qrc

FORMS += \
    $$PWD/avatarsettingsform.ui \
    $$PWD/badgesettingsform.ui \
    $$PWD/checkboxsettingsform.ui \
    $$PWD/fabsettingsform.ui \
    $$PWD/flatbuttonsettingsform.ui \
    $$PWD/iconbuttonsettingsform.ui \
    $$PWD/progresssettingsform.ui \
    $$PWD/circularprogresssettingsform.ui \
    $$PWD/slidersettingsform.ui \
    $$PWD/snackbarsettingsform.ui \
    $$PWD/radiobuttonsettingsform.ui \
    $$PWD/togglesettingsform.ui \
    $$PWD/textfieldsettingsform.ui \
    $$PWD/tabssettingsform.ui \
    $$PWD/dialogsettingsform.ui \
    $$PWD/drawersettingsform.ui \
    $$PWD/scrollbarsettingsform.ui \
    $$PWD/appbarsettingsform.ui
