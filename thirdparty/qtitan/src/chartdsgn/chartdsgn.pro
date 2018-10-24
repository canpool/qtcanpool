TEMPLATE    = lib

TARGET      = qtnchartdsgn

!debug_and_release|build_pass {
    CONFIG(debug, debug|release) {
        TARGET = $$member(TARGET, 0)d
    }
}

QTITANDIR = $$(QTITANDIR)
include($$QTITANDIR/src/shared/platform.pri)

DESTDIR = $$PWD/../../lib
DESTDIR = $$member(DESTDIR, 0)$$QTITAN_LIB_PREFIX

VERSION = 2.1

QMAKE_TARGET_COMPANY = Developer Machines
QMAKE_TARGET_PRODUCT = QtitanChart
QMAKE_TARGET_DESCRIPTION = QtitanChart plugin for QtDesigner
QMAKE_TARGET_COPYRIGHT = Copyright (C) 2009-2013 Developer Machines

contains(CONFIG, demo) {
    QMAKE_TARGET_DESCRIPTION = QtitanChart plugin for QtDesigner (Demo version)
}

include($$QTITANDIR/src/chart/chart.pri)

HEADERS     += $$PWD/QtnChartDsgnPlugin.h \
               $$PWD/QtnPlugins.h

SOURCES     += $$PWD/QtnChartDsgnPlugin.cpp \
               $$PWD/QtnPlugins.cpp

RESOURCES += \
    $$PWD/QtnChartDsgnResources.qrc

DEFINES += QTITAN_LIBRARY_STATIC
DEFINES += QTITAN_DESIGNER

CONFIG += plugin

!lessThan(QT_VER_MAJ, 5) {
  QT += widgets designer
} else {
  DLLDESTDIR = $$PWD/../../bin
  DLLDESTDIR = $$member(DLLDESTDIR, 0)$$QTITAN_LIB_PREFIX
  CONFIG += designer
}

