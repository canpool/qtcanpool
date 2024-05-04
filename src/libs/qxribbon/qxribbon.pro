include(../../qtlibrary.pri)
include(qxribbon-lib.pri)

# deploy header files
qxribbon.files = $$PUBLIC_HEADERS
qxribbon.path = $$IDE_INC_PATH/qxribbon
INSTALLS += qxribbon
