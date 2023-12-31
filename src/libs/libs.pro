include(../../qtproject.pri)

TEMPLATE  = subdirs

SUBDIRS =   \
    qcanpool \
    qxribbon \
    qxframeless \
    qxwindow \
    qgood \
    qgoodbye \
    qcustomplot

lessThan(QT_MAJOR_VERSION, 6) {
    SUBDIRS += \
        qribbon \
        qxmaterial
}

QTC_BUILD_EXTENSIONSYSTEM = 0
equals(QTC_BUILD_EXTENSIONSYSTEM, 1) {
    SUBDIRS += \
        aggregation \
        extensionsystem \
        utils
}

for(l, SUBDIRS) {
    QTC_LIB_DEPENDS =
    include($$l/$${l}_dependencies.pri)
    lv = $${l}.depends
    $$lv = $$QTC_LIB_DEPENDS
}
