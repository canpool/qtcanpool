TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = \
    helloworld \
    qcanpool \
    qxwidget \
    qxwindow \
    qgood \
    application \
    misc \

lessThan(QT_MAJOR_VERSION, 6) {
    SUBDIRS += qribbon
}

DEMO_THIRDPARTY_ENABLE = 0
equals(DEMO_THIRDPARTY_ENABLE, 1) {
    SUBDIRS += thirdparty
}
