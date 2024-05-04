include(../../qtlibrary.pri)
include(qcanpool-lib.pri)

# deploy header files
qcanpool.files = $$PUBLIC_HEADERS
qcanpool.path = $$IDE_INC_PATH/qcanpool
INSTALLS += qcanpool
