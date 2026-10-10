include(../../qtlibrary.pri)
include(qxapp-lib.pri)

# deploy header files
qxapp.files = $$PUBLIC_HEADERS
qxapp.path = $$IDE_INC_PATH/qxapp
INSTALLS += qxapp

CONFIG(release, debug|release) {
    COPIES += qxapp
}
