include(../../qtlibrary.pri)

QX_WINDOW_CONFIG_NATIVE = 1
QX_WINDOW_CONFIG_ENABLE_SYSTEM_BORDERS = 1
QX_WINDOW_CONFIG_ENABLE_STYLE_AGENT = 0

include(qxwindow-lib.pri)

# deploy header files
qxwindow.files = $$PUBLIC_HEADERS
qxwindow.path = $$IDE_INC_PATH/qxwindow
INSTALLS += qxwindow

CONFIG(release, debug|release) {
    COPIES += qxwindow
}
