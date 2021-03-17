include(qtproject.pri)

#version check qt
!minQtVersion(5, 0, 0) {
    message("Cannot build $${IDE_DISPLAY_NAME} with Qt version $${QT_VERSION}.")
    error("Can only use Qt 5.")
}

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = \
    src \
    share \
    demos
