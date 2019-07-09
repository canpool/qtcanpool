DEFINES += VCSBASE_LIBRARY

HEADERS += \
    $$PWD/vcsbase_global.h \
    $$PWD/vcsbaseconstants.h \
    $$PWD/vcsprojectcache.h \
    $$PWD/wizard/vcsconfigurationpage.h \
    $$PWD/wizard/vcscommandpage.h \
    $$PWD/wizard/vcsjsextension.h \
    $$PWD/vcsplugin.h \
    $$PWD/vcsbaseplugin.h \
    $$PWD/baseannotationhighlighter.h \
    $$PWD/diffandloghighlighter.h \
    $$PWD/vcsbaseeditor.h \
    $$PWD/vcsbasesubmiteditor.h \
    $$PWD/basevcseditorfactory.h \
    $$PWD/submiteditorfile.h \
    $$PWD/basevcssubmiteditorfactory.h \
    $$PWD/submitfilemodel.h \
    $$PWD/commonvcssettings.h \
    $$PWD/commonsettingspage.h \
    $$PWD/nicknamedialog.h \
    $$PWD/vcsoutputwindow.h \
    $$PWD/cleandialog.h \
    $$PWD/vcsbaseoptionspage.h \
    $$PWD/vcscommand.h \
    $$PWD/vcsbaseclient.h \
    $$PWD/vcsbaseclientsettings.h \
    $$PWD/vcsbaseeditorconfig.h \
    $$PWD/submitfieldwidget.h \
    $$PWD/submiteditorwidget.h \
    $$PWD/vcsbasediffeditorcontroller.h

SOURCES += \
    $$PWD/vcsplugin.cpp \
    $$PWD/vcsbaseplugin.cpp \
    $$PWD/vcsprojectcache.cpp \
    $$PWD/wizard/vcsconfigurationpage.cpp \
    $$PWD/wizard/vcscommandpage.cpp \
    $$PWD/wizard/vcsjsextension.cpp \
    $$PWD/baseannotationhighlighter.cpp \
    $$PWD/diffandloghighlighter.cpp \
    $$PWD/vcsbaseeditor.cpp \
    $$PWD/vcsbasesubmiteditor.cpp \
    $$PWD/basevcseditorfactory.cpp \
    $$PWD/submiteditorfile.cpp \
    $$PWD/basevcssubmiteditorfactory.cpp \
    $$PWD/submitfilemodel.cpp \
    $$PWD/commonvcssettings.cpp \
    $$PWD/commonsettingspage.cpp \
    $$PWD/nicknamedialog.cpp \
    $$PWD/vcsoutputwindow.cpp \
    $$PWD/cleandialog.cpp \
    $$PWD/vcsbaseoptionspage.cpp \
    $$PWD/vcscommand.cpp \
    $$PWD/vcsbaseclient.cpp \
    $$PWD/vcsbaseclientsettings.cpp \
    $$PWD/vcsbaseeditorconfig.cpp \
    $$PWD/submitfieldwidget.cpp \
    $$PWD/submiteditorwidget.cpp \
    $$PWD/vcsbasediffeditorcontroller.cpp

RESOURCES += $$PWD/vcsbase.qrc

FORMS += \
    $$PWD/commonsettingspage.ui \
    $$PWD/nicknamedialog.ui \
    $$PWD/cleandialog.ui \
    $$PWD/submiteditorwidget.ui

equals(TEST, 1): DEFINES += "SRC_DIR=\\\"$$IDE_SOURCE_TREE\\\""
