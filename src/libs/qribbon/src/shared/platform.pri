QT_VERSION = $$[QT_VERSION]
QT_VERSION = $$split(QT_VERSION, ".")
QT_VER_MAJ = $$member(QT_VERSION, 0)
QT_VER_MIN = $$member(QT_VERSION, 1)

!lessThan(QT_VER_MAJ, 5) {
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

