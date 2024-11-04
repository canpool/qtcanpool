include(../../qtlibrary.pri)
include(qxdock-lib.pri)

# deploy header files
qxdock.files = $$PUBLIC_HEADERS
qxdock.path = $$IDE_INC_PATH/qxdock
INSTALLS += qxdock
