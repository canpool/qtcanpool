include(../../qtproject.pri)

TEMPLATE  = subdirs

SUBDIRS =   \
    qcanpool \
    qxwidget \
    qxribbon

lessThan(QT_MAJOR_VERSION, 6) {
    SUBDIRS += qribbon
}

for(l, SUBDIRS) {
    QTC_LIB_DEPENDS =
    include($$l/$${l}_dependencies.pri)
    lv = $${l}.depends
    $$lv = $$QTC_LIB_DEPENDS
}
