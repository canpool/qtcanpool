include(../../qtproject.pri)

TEMPLATE  = subdirs

SUBDIRS =   \
    aggregation \
    extensionsystem \
    utils \
    qcanpool \
    qribbon

exists($$PWD/qads/Qt-Advanced-Docking-System/src) {
    SUBDIRS += qads
}

for(l, SUBDIRS) {
    QTC_LIB_DEPENDS =
    include($$l/$${l}_dependencies.pri)
    lv = $${l}.depends
    $$lv = $$QTC_LIB_DEPENDS
}
