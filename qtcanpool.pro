include(qtproject.pri)

!minQtVersion(5, 0, 0) {
    message("Cannot build $${IDE_DISPLAY_NAME} with Qt version $${QT_VERSION}.")
    error("Can only use Qt 5.")
}

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = \
    src \
    demos \
    examples \
    tests
