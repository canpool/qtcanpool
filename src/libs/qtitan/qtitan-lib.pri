VERSION = 1.0.0

greaterThan(QT_MAJOR_VERSION, 4) {
  QT += widgets core-private widgets-private gui-private
}

DEFINES += QTITAN_LIBRARY

QMAKE_TARGET_COMPANY = Developer Machines
QMAKE_TARGET_PRODUCT = QtitanComponents
QMAKE_TARGET_DESCRIPTION = Qtitan UI component for Digia Qt.C++
QMAKE_TARGET_COPYRIGHT = Copyright (C) 2009-2015 Developer Machines

include($$PWD/src/shared/platform.pri)
include($$PWD/src/styles/styles.pri)
include($$PWD/src/ribbon/ribbon.pri)

# Ribbon
COMPONENT_LANGUAGES = zh_hans
COMPONENT_SOURCE_TREE=$$PWD/src/ribbon
COMPONENT_TRANSLATIONS_DIR=$$PWD/translations/ribbon

include($$PWD/src/shared/translations.pri)
