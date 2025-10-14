TEMPLATE  = subdirs

SUBDIRS =   \
    qcanpool \
    qxribbon \
    qxdock \
    qxwindow \
    qxwidget

for(l, SUBDIRS) {
    QTC_LIB_DEPENDS =
    include($$l/$${l}_dependencies.pri)
    lv = $${l}.depends
    $$lv = $$QTC_LIB_DEPENDS
}
