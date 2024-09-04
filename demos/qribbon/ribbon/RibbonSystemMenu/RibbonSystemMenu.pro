QTC_LIB_DEPENDS += qribbon
include(../../../../qtproject.pri)

TEMPLATE = app
DESTDIR = $$IDE_APP_PATH

include($$PWD/RibbonSystemMenu.pri)
include($$PWD/../../shared/aboutdialog.pri)
