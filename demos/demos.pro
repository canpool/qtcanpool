TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = \
    qcanpool \
    qxwidget \
    qxwindow

lessThan(QT_MAJOR_VERSION, 6) {
    SUBDIRS += qribbon
}

DEMO_OTHERS_ENABLE = 0
equals(DEMO_OTHERS_ENABLE, 1) {
    SUBDIRS += helloworld
}

DEMO_THIRDPARTY_ENABLE = 0
equals(DEMO_THIRDPARTY_ENABLE, 1) {
    SUBDIRS += thirdparty
}
