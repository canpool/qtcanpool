TEMPLATE = subdirs

include(src/shared/platform.pri)

CONFIG += ordered

QTN_BUILD_SOURCE = 0
QTN_COMPONENT_VERSION = $$fromfile($$PWD/src/ribbon/ribbon.pri, QTN_COMPONENT_VERSION)

exists($$PWD/src/shared/ribbon/ribbon.pro) {
    !isEmpty(QTN_COMPONENT_VERSION):QTN_BUILD_SOURCE = 1
}

equals(QTN_BUILD_SOURCE, 1) {
    !build_pass {
        message(Qtitan source code being prepared for building...)
    }

    SUBDIRS = \
        src/shared/ribbon \
        src/ribbondsgn

    SUBDIRS += \
        demos/ribbon \
        demos/styles
}

exists($$PWD/doc/qdocs.pri) {
    include($$PWD/doc/qdocs.pri)
}
