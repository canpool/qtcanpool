
QTITANDIR = $$quote($$(QTITANDIR))

include($$QTITANDIR/src/shared/platform.pri)
include($$QTITANDIR/src/ribbon/ribbon.pri)
include($$QTITANDIR/src/styles/styles.pri)

DEFINES += QTITAN_LIBRARY_STATIC

