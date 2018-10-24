TEMPLATE    = lib

TARGET      = qtnribbondsgn

!debug_and_release|build_pass {
    CONFIG(debug, debug|release) {
        TARGET = $$member(TARGET, 0)d
    }
}

QTITANDIR = $$(QTITANDIR)
include($$QTITANDIR/src/shared/platform.pri)

DESTDIR = $$PWD/../../lib
DESTDIR = $$member(DESTDIR, 0)$$QTITAN_LIB_PREFIX

VERSION = 3.0.0

QMAKE_TARGET_COMPANY = Developer Machines
QMAKE_TARGET_PRODUCT = QtitanRibbon for Qt
QMAKE_TARGET_DESCRIPTION = QtitanRibbon Plugin for QtDesigner
QMAKE_TARGET_COPYRIGHT = Copyright (C) 2009-2013 Developer Machines

contains(CONFIG, demo) {
    QMAKE_TARGET_DESCRIPTION = QtitanRibbon Plugin for QtDesigner (Demo version)
}

include($$QTITANDIR/src/ribbon/ribbon.pri)
include($$QTITANDIR/src/styles/styles.pri)

HEADERS     += $$QTITANDIR/src/ribbondsgn/QtnRibbonDsgnPlugin.h \
               $$QTITANDIR/src/ribbondsgn/QtnRibbonDsgnContainer.h \
               $$QTITANDIR/src/ribbondsgn/QtnRibbonDsgnTaskMenu.h \
               $$QTITANDIR/src/ribbondsgn/QtnRibbonPageDsgnPlugin.h \
               $$QTITANDIR/src/ribbondsgn/QtnRibbonPlugins.h \
               $$QTITANDIR/src/ribbondsgn/QtnMainWindowDsgnPlugin.h \
               $$QTITANDIR/src/ribbondsgn/QtnMainWindowDsgnContainer.h \
               $$QTITANDIR/src/ribbondsgn/QtnMainWindowDsgnTaskMenu.h \
               $$QTITANDIR/src/ribbondsgn/QtnStatusBarDsgnPlugin.h \
               $$QTITANDIR/src/ribbondsgn/QtnCommandDsgn.h \
               $$QTITANDIR/src/ribbondsgn/QtnRibbonBackstageViewPlugin.h \
               $$QTITANDIR/src/ribbondsgn/QtnRibbonStyleDsgnPlugin.h

SOURCES     += $$QTITANDIR/src/ribbondsgn/QtnRibbonDsgnPlugin.cpp \
               $$QTITANDIR/src/ribbondsgn/QtnRibbonDsgnContainer.cpp \
               $$QTITANDIR/src/ribbondsgn/QtnRibbonDsgnTaskMenu.cpp \
               $$QTITANDIR/src/ribbondsgn/QtnRibbonPageDsgnPlugin.cpp \
               $$QTITANDIR/src/ribbondsgn/QtnRibbonPlugins.cpp \
               $$QTITANDIR/src/ribbondsgn/QtnMainWindowDsgnPlugin.cpp \
               $$QTITANDIR/src/ribbondsgn/QtnMainWindowDsgnContainer.cpp \
               $$QTITANDIR/src/ribbondsgn/QtnMainWindowDsgnTaskMenu.cpp \
               $$QTITANDIR/src/ribbondsgn/QtnStatusBarDsgnPlugin.cpp \
               $$QTITANDIR/src/ribbondsgn/QtnCommandDsgn.cpp \
               $$QTITANDIR/src/ribbondsgn/QtnRibbonBackstageViewPlugin.cpp \
               $$QTITANDIR/src/ribbondsgn/QtnRibbonStyleDsgnPlugin.cpp

RESOURCES += \
    $$PWD/QtnRibbonDsgnResources.qrc

CONFIG += plugin

!lessThan(QT_VER_MAJ, 5) {
  QT += widgets designer core-private widgets-private gui-private designer
} else {
  CONFIG      += designer
  DLLDESTDIR = $$PWD/../../bin
  DLLDESTDIR = $$member(DLLDESTDIR, 0)$$QTITAN_LIB_PREFIX
}

DEFINES += QTITAN_LIBRARY_STATIC
DEFINES += QTITAN_DESIGNER


