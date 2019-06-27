include($$PWD/../../library.pri)

DEFINES += AGGREGATION_LIBRARY
message($$DESTDIR)
HEADERS = aggregate.h \
    aggregation_global.h

SOURCES = aggregate.cpp

