TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = \
    helloworld \
    qcanpool \
    qxwidget \
    qrcodedemo \
    controldemo \

lessThan(QT_MAJOR_VERSION, 6) {
    SUBDIRS += qribbon
}
