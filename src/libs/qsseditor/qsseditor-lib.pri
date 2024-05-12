VERSION = 0.7.0

DEFINES += QSSEDITOR_SHARED_EXPORT

# remove some macros in qtproject.pri
DEFINES -= \
    QT_NO_JAVA_STYLE_ITERATORS \
    QT_NO_CAST_TO_ASCII \
    QT_RESTRICTED_CAST_FROM_ASCII \
    "QT_DISABLE_DEPRECATED_BEFORE=0x050900"

include($$PWD/qsseditor-src/qsseditor-src.pri)
