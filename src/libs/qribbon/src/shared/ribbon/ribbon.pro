TEMPLATE = lib

TARGET = qtnribbon

!debug_and_release|build_pass {
    CONFIG(debug, debug|release) {
        TARGET = $$member(TARGET, 0)d
    }
}

include($$PWD/../platform.pri)

DESTDIR = $$PWD/../../../lib
DESTDIR = $$member(DESTDIR, 0)$$QTITAN_LIB_PREFIX
contains(QMAKE_HOST.arch, x86_64) {
    DESTDIR = $$DESTDIR/X64
}

VERSION = 3.1.0

QMAKE_TARGET_COMPANY = Developer Machines
QMAKE_TARGET_PRODUCT = QtitanRibbon user interface component for Qt
QMAKE_TARGET_DESCRIPTION = QtitanRibbon user interface component for Qt
QMAKE_TARGET_COPYRIGHT = Copyright (C) 2009-2013 Developer Machines

include($$PWD/../../ribbon/ribbon.pri)
include($$PWD/../../styles/styles.pri)

contains(CONFIG, demo) {
include($$PWD/../qtitandemo.pri)
}

!lessThan(QT_VER_MAJ, 5) {
  QT += widgets core-private widgets-private gui-private
} else {
  DLLDESTDIR = $$PWD/../../../bin
  DLLDESTDIR = $$member(DLLDESTDIR, 0)$$QTITAN_LIB_PREFIX
}

DEFINES += QTITAN_LIBRARY


