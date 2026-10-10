TEMPLATE  = subdirs

SUBDIRS =   \
    qxribbon \
    qxdock \
    qxwindow \
    qxapp

for(l, SUBDIRS) {
    QTC_LIB_DEPENDS =
    include($$l/$${l}_dependencies.pri)
    lv = $${l}.depends
    $$lv = $$QTC_LIB_DEPENDS
}
