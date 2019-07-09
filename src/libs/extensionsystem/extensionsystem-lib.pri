DEFINES += EXTENSIONSYSTEM_LIBRARY

unix:LIBS += $$QMAKE_LIBS_DYNLOAD

!isEmpty(vcproj) {
    DEFINES += IDE_TEST_DIR=\"$$IDE_SOURCE_TREE\"
} else {
    DEFINES += IDE_TEST_DIR=\\\"$$IDE_SOURCE_TREE\\\"
}

HEADERS += \
    $$PWD/pluginerrorview.h \
    $$PWD/plugindetailsview.h \
    $$PWD/invoker.h \
    $$PWD/iplugin.h \
    $$PWD/iplugin_p.h \
    $$PWD/extensionsystem_global.h \
    $$PWD/pluginmanager.h \
    $$PWD/pluginmanager_p.h \
    $$PWD/pluginspec.h \
    $$PWD/pluginspec_p.h \
    $$PWD/pluginview.h \
    $$PWD/optionsparser.h \
    $$PWD/pluginerroroverview.h

SOURCES += \
    $$PWD/pluginerrorview.cpp \
    $$PWD/plugindetailsview.cpp \
    $$PWD/invoker.cpp \
    $$PWD/iplugin.cpp \
    $$PWD/pluginmanager.cpp \
    $$PWD/pluginspec.cpp \
    $$PWD/pluginview.cpp \
    $$PWD/optionsparser.cpp \
    $$PWD/pluginerroroverview.cpp

FORMS += \
    $$PWD/pluginerrorview.ui \
    $$PWD/plugindetailsview.ui \
    $$PWD/pluginerroroverview.ui
