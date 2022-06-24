TEMPLATE = app

TARGET = RibbonDemo

isEmpty(QTPROJECT_DIR):             QTPROJECT_DIR = $$PWD/..
isEmpty(QTPROJECT_OUT_PWD):         QTPROJECT_OUT_PWD = $$OUT_PWD/..

include(../qtitanconfig.pri)

include($$QTITANDIR/src/shared/qtitanribbon.pri)

DESTDIR = $$QTN_OUTPUT_DIR

!debug_and_release|build_pass {
    CONFIG(debug, debug|release) {
        TARGET = $$member(TARGET, 0)d
    }
}

include(src/src.pri)

macx {
    CONFIG-=app_bundle
}

!lessThan(QT_VER_MAJ, 5) {
  QT += printsupport
}
