include(../../qtlibrary.pri)

win32 {
    QX_FRAMELESS_CONFIG_NATIVE = 1
}
include($$PWD/qxframeless-lib.pri)
