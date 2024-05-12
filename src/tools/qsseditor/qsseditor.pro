QTC_LIB_DEPENDS += qsseditor

include(../../qttool.pri)

TARGET = QssEditor

include($$QTCANPOOL_DIR/src/libs/qsseditor/qsseditor-inc.pri)

SOURCES += \
    main.cpp
