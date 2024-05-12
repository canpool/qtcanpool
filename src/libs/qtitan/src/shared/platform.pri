QT_VERSION = $$[QT_VERSION]
QT_VERSION = $$split(QT_VERSION, ".")
QT_VER_MAJ = $$member(QT_VERSION, 0)
QT_VER_MIN = $$member(QT_VERSION, 1)

defineReplace(toNativeSeparators) {
    return($$replace(1, /, $$QMAKE_DIR_SEP))
}
defineReplace(fromNativeSeparators) {
    return($$replace(1, \\\\, /))
}

defineReplace(cleanPath) {
    win32:1 ~= s|\\\\|/|g
    contains(1, ^/.*):pfx = /
    else:pfx =
    segs = $$split(1, /)
    out =
    for(seg, segs) {
        equals(seg, ..):out = $$member(out, 0, -2)
        else:!equals(seg, .):out += $$seg
    }
    return($$join(out, /, $$pfx))
}

RCC = $$toNativeSeparators($$cleanPath($$[QT_INSTALL_BINS]/rcc))
LRELEASE = $$toNativeSeparators($$cleanPath($$[QT_INSTALL_BINS]/lrelease))
LUPDATE = $$toNativeSeparators($$cleanPath($$[QT_INSTALL_BINS]/lupdate))
LCONVERT = $$toNativeSeparators($$cleanPath($$[QT_INSTALL_BINS]/lconvert))
QMAKE_BINARY = $$toNativeSeparators($$cleanPath($$[QT_INSTALL_BINS]/qmake))

win32 {
    RCC = $${RCC}.exe
    LRELEASE = $${LRELEASE}.exe
    LUPDATE = $${LUPDATE}.exe
    LCONVERT = $${LCONVERT}.exe
    QMAKE_BINARY = $${QMAKE_BINARY}.exe
}

!lessThan(QT_VER_MAJ, 5) {
}

win32-msvc2012 {
      QTITAN_LIB_PREFIX=
}

win32-msvc2010 {
      QTITAN_LIB_PREFIX=
}

win32-msvc2008 {
      QTITAN_LIB_PREFIX=
}

win32-msvc2005 {
      QTITAN_LIB_PREFIX=
}

win32-g++ {
      QTITAN_LIB_PREFIX=
}

QTITAN_ROOT = $$PWD/../..
QTN_OUTPUT_PREFIX = $$PWD/../..
isEmpty(QTCANPOOL_DIR) {
    include($$QTITAN_ROOT/qtproject.pri)
    QTN_OUTPUT_PREFIX = $$IDE_BUILD_TREE
}
isEmpty(IDE_BUILD_TREE) {
    IDE_BUILD_TREE = $$QTITAN_ROOT
}
QTN_OUTPUT_DIR = $$QTN_OUTPUT_PREFIX/bin$$QTITAN_LIB_PREFIX
