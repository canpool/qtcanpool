TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = \
    helloworld \
    qcanpool \
    qxwidget \
    plotdemo \
    qrcodedemo \
    controldemo \

lessThan(QT_MAJOR_VERSION, 6) {
    SUBDIRS += qribbon
}
