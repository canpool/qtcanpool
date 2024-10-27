TEMPLATE  = subdirs

SUBDIRS =   \
    qcanpool \
    qxribbon \
    qxmaterial \
    qxwindow

lessThan(QT_MAJOR_VERSION, 6) {
    exists($$PWD/qtitan/qtitan.pro) {
        SUBDIRS += \
            qtitan
    }
}

for(l, SUBDIRS) {
    QTC_LIB_DEPENDS =
    include($$l/$${l}_dependencies.pri)
    lv = $${l}.depends
    $$lv = $$QTC_LIB_DEPENDS
}
