include($$PWD/src/shared/platform.pri)

VERSION = 3.1.1

QMAKE_TARGET_COMPANY = Developer Machines
QMAKE_TARGET_PRODUCT = QtitanRibbon user interface component for Qt
QMAKE_TARGET_DESCRIPTION = QtitanRibbon user interface component for Qt
QMAKE_TARGET_COPYRIGHT = Copyright (C) 2009-2013 Developer Machines

include($$PWD/src/ribbon/ribbon.pri)
include($$PWD/src/styles/styles.pri)

!lessThan(QT_VER_MAJ, 5) {
  QT += widgets core-private widgets-private gui-private
}

DEFINES += QTITAN_LIBRARY
