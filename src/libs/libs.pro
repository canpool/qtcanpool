TEMPLATE  = subdirs

SUBDIRS =   \
    qcanpool \
    qxribbon \
    qxmaterial \
    qxwindow

lessThan(QT_MAJOR_VERSION, 6) {
    SUBDIRS += \
        qribbon

    exists($$PWD/qtitan/qtitan.pro) {
        SUBDIRS += \
            qtitan
    }
}

exists($$PWD//qsseditor/qsseditor-src/qsseditor-src.pri) {
    SUBDIRS += qsseditor
}

for(l, SUBDIRS) {
    QTC_LIB_DEPENDS =
    include($$l/$${l}_dependencies.pri)
    lv = $${l}.depends
    $$lv = $$QTC_LIB_DEPENDS
}
