TEMPLATE = app

QTITANDIR = $$quote($$(QTITANDIR))

isEmpty(QTITANDIR):QTITANDIR = $$quote($$PWD/../../..)

include($$QTITANDIR/src/shared/qtitanchart.pri)

#DESTDIR = $$QTITANDIR/bin
DESTDIR = $$IDE_APP_PATH
DESTDIR = $$member(DESTDIR, 0)$$QTITAN_LIB_PREFIX
DESTDIR = "$$DESTDIR"

# auto copy dll to STATIC_OUTPUT_BASE
STATIC_OUTPUT_BASE = $$DESTDIR
include($$QTITANDIR/../../data.pri)

!debug_and_release|build_pass {
    CONFIG(debug, debug|release) {
        TARGET = $$member(TARGET, 0)d
    }
}

HEADERS = pyramidcharts.h

SOURCES = main.cpp \
          pyramidcharts.cpp



include($$QTITANDIR/demos/shared/demomainwindow.pri)
include($$QTITANDIR/demos/chart/shared/chartwindow.pri)

macx {
    CONFIG-=app_bundle
}

