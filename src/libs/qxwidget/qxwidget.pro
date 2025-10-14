include(../../qtlibrary.pri)
include(qxwidget-lib.pri)

# deploy header files
qxwidget.files = $$PUBLIC_HEADERS
qxwidget.path = $$IDE_INC_PATH/qxwidget
INSTALLS += qxwidget

CONFIG(release, debug|release) {
    COPIES += qxwidget
}
