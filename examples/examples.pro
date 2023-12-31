TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = \
    qcanpool \
    qxribbon

lessThan(QT_MAJOR_VERSION, 6) {
    SUBDIRS += qxmaterial
}
