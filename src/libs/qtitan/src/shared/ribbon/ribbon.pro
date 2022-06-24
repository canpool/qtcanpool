TEMPLATE = lib

TARGET = qtnribbon

!debug_and_release|build_pass {
    CONFIG(debug, debug|release) {
        TARGET = $$member(TARGET, 0)d
    }
}

include($$PWD/../platform.pri)
include($$PWD/../../styles/styles.pri)
include($$PWD/../../ribbon/ribbon.pri)

contains(CONFIG, demo) {
include($$PWD/../qtitandemo.pri)
}

DESTDIR = $$QTN_OUTPUT_DIR

VERSION = $$QTN_COMPONENT_VERSION

QMAKE_TARGET_COMPANY = Developer Machines
QMAKE_TARGET_PRODUCT = QtitanRibbon
QMAKE_TARGET_DESCRIPTION = QtitanRibbon UI component for Digia Qt.C++
QMAKE_TARGET_COPYRIGHT = Copyright (C) 2009-2015 Developer Machines

!lessThan(QT_VER_MAJ, 5) {
  QT += widgets core-private widgets-private gui-private
} else {
  DLLDESTDIR = $$QTN_OUTPUT_DIR
}

QTITAN_ROOT=$$PWD/../../..
COMPONENT_LANGUAGES = zh_hans
COMPONENT_SOURCE_TREE=$$QTITAN_ROOT/src/ribbon
COMPONENT_TRANSLATIONS_DIR=$$QTITAN_ROOT/translations/ribbon

include($$QTITAN_ROOT/src/shared/translations.pri)

DEFINES += QTITAN_LIBRARY


