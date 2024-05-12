TEMPLATE = subdirs

SUBDIRS =

exists($$PWD/../libs/qsseditor/qsseditor-src/qsseditor-src.pri) {
    SUBDIRS += qsseditor
}
