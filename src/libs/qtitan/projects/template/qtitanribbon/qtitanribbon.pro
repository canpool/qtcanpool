isEmpty(QTPROJECT_DIR):             QTPROJECT_DIR = $$PWD/..
isEmpty(QTPROJECT_OUT_PWD):         QTPROJECT_OUT_PWD = $$OUT_PWD/..

include(../qtitanconfig.pri)
include($$QTITANDIR/src/shared/ribbon/ribbon.pro)
