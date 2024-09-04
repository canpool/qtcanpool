QTC_LIB_DEPENDS += qribbon
include(../../../../qtproject.pri)

TEMPLATE = app
TARGET = RibbonApplication
DESTDIR = $$IDE_APP_PATH

DEFINES += QT_DEPRECATED_WARNINGS

include($$PWD/RibbonApplication.pri)
include($$PWD/../../ribbon/shared/ribbonwindow.pri)
include($$PWD/../../shared/aboutdialog.pri)
