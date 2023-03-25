DEFINES += CORE_LIBRARY

# embedding build time information prevents repeatedly binary exact versions from same source code
isEmpty(QTC_SHOW_BUILD_DATE): QTC_SHOW_BUILD_DATE = $$(QTC_SHOW_BUILD_DATE)
!isEmpty(QTC_SHOW_BUILD_DATE): DEFINES += QTC_SHOW_BUILD_DATE

include(../../qtplugin.pri)
msvc: QMAKE_CXXFLAGS += -wd4251 -wd4290 -wd4250
SOURCES += mainwindow.cpp \
    coreplugin.cpp \

HEADERS += mainwindow.h \
    coreplugin.h \

RESOURCES += core.qrc
