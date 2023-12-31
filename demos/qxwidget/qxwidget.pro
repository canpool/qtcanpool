TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = \
    ribbondemo

lessThan(QT_MAJOR_VERSION, 6) {
    SUBDIRS += \
        materialdemo
}
