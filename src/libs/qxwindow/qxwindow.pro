include(../../qtlibrary.pri)

win32-g++ {
    QX_WINDOW_CONFIG_NATIVE = 0
} else {
    QX_WINDOW_CONFIG_NATIVE = 1
}
QX_WINDOW_CONFIG_ENABLE_SYSTEM_BORDERS = 1
QX_WINDOW_CONFIG_ENABLE_STYLE_AGENT = 0

include(qxwindow-lib.pri)
