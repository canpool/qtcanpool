QTC_LIB_DEPENDS += qxmaterial
include(../../../qtproject.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = Example_MaterialComponent
TEMPLATE = app
DESTDIR = $$IDE_APP_PATH

SOURCES = \
    mainwindow.cpp \
    main.cpp \
    avatarsettingseditor.cpp \
    badgesettingseditor.cpp \
    checkboxsettingseditor.cpp \
    fabsettingseditor.cpp \
    raisedbuttonsettingseditor.cpp \
    flatbuttonsettingseditor.cpp \
    iconbuttonsettingseditor.cpp \
    progresssettingseditor.cpp \
    circularprogresssettingseditor.cpp \
    slidersettingseditor.cpp \
    radiobuttonsettingseditor.cpp \
    togglesettingseditor.cpp \
    textfieldsettingseditor.cpp \
    tabssettingseditor.cpp \
    snackbarsettingseditor.cpp \
    dialogsettingseditor.cpp \
    drawersettingseditor.cpp \
    scrollbarsettingseditor.cpp \
    appbarsettingseditor.cpp \
    autocompletesettingseditor.cpp

HEADERS = \
    mainwindow.h \
    avatarsettingseditor.h \
    badgesettingseditor.h \
    checkboxsettingseditor.h \
    fabsettingseditor.h \
    raisedbuttonsettingseditor.h \
    flatbuttonsettingseditor.h \
    iconbuttonsettingseditor.h \
    progresssettingseditor.h \
    circularprogresssettingseditor.h \
    slidersettingseditor.h \
    radiobuttonsettingseditor.h \
    togglesettingseditor.h \
    textfieldsettingseditor.h \
    tabssettingseditor.h \
    snackbarsettingseditor.h \
    dialogsettingseditor.h \
    drawersettingseditor.h \
    scrollbarsettingseditor.h \
    appbarsettingseditor.h \
    autocompletesettingseditor.h

RESOURCES += \
    examples.qrc

FORMS += \
    avatarsettingsform.ui \
    badgesettingsform.ui \
    checkboxsettingsform.ui \
    fabsettingsform.ui \
    flatbuttonsettingsform.ui \
    iconbuttonsettingsform.ui \
    progresssettingsform.ui \
    circularprogresssettingsform.ui \
    slidersettingsform.ui \
    snackbarsettingsform.ui \
    radiobuttonsettingsform.ui \
    togglesettingsform.ui \
    textfieldsettingsform.ui \
    tabssettingsform.ui \
    dialogsettingsform.ui \
    drawersettingsform.ui \
    scrollbarsettingsform.ui \
    appbarsettingsform.ui
