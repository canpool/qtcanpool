TEMPLATE = lib

TARGET = qtnchart

!debug_and_release|build_pass {
    CONFIG(debug, debug|release) {
        TARGET = $$member(TARGET, 0)d
    }
}

QTITANDIR = $$quote($$(QTITANDIR))

include($$QTITANDIR/src/shared/platform.pri)

DESTDIR = $$QTITANDIR/lib
DESTDIR = $$member(DESTDIR, 0)$$QTITAN_LIB_PREFIX
contains(QMAKE_HOST.arch, x86_64) {
    DESTDIR = $$DESTDIR/X64
}

VERSION = 2.1

QMAKE_TARGET_COMPANY = Developer Machines
QMAKE_TARGET_PRODUCT = Qtitan Chart user interface component for Qt
QMAKE_TARGET_DESCRIPTION = Qtitan Chart user interface component for Qt
QMAKE_TARGET_COPYRIGHT = Copyright (C) 2009-2013 Developer Machines

include($$PWD/../../chart/chart.pri)

contains(CONFIG, demo) {
include($$PWD/../qtitandemo.pri)
}

!lessThan(QT_VER_MAJ, 5) {
  QT += widgets
} else {
  DLLDESTDIR = $$QTITANDIR/bin
  DLLDESTDIR = $$member(DLLDESTDIR, 0)$$QTITAN_LIB_PREFIX
}

DEFINES += QTITAN_LIBRARY


