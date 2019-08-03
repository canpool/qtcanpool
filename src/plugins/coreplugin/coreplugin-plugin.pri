DEFINES += CORE_LIBRARY
QT += \
    network \
    printsupport \
    qml \
    sql

# embedding build time information prevents repeatedly binary exact versions from same source code
isEmpty(QTC_SHOW_BUILD_DATE): QTC_SHOW_BUILD_DATE = $$(QTC_SHOW_BUILD_DATE)
!isEmpty(QTC_SHOW_BUILD_DATE): DEFINES += QTC_SHOW_BUILD_DATE

msvc: QMAKE_CXXFLAGS += -wd4251 -wd4290 -wd4250
SOURCES += \
    $$PWD/corejsextensions.cpp \
    $$PWD/mainwindow.cpp \
    $$PWD/shellcommand.cpp \
    $$PWD/editmode.cpp \
    $$PWD/iwizardfactory.cpp \
    $$PWD/fancyactionbar.cpp \
    $$PWD/fancytabwidget.cpp \
    $$PWD/generalsettings.cpp \
    $$PWD/themechooser.cpp \
    $$PWD/id.cpp \
    $$PWD/icontext.cpp \
    $$PWD/jsexpander.cpp \
    $$PWD/messagemanager.cpp \
    $$PWD/messageoutputwindow.cpp \
    $$PWD/outputpane.cpp \
    $$PWD/outputwindow.cpp \
    $$PWD/vcsmanager.cpp \
    $$PWD/statusbarmanager.cpp \
    $$PWD/versiondialog.cpp \
    $$PWD/editormanager/editorarea.cpp \
    $$PWD/editormanager/editormanager.cpp \
    $$PWD/editormanager/editorview.cpp \
    $$PWD/editormanager/editorwindow.cpp \
    $$PWD/editormanager/documentmodel.cpp \
    $$PWD/editormanager/openeditorsview.cpp \
    $$PWD/editormanager/openeditorswindow.cpp \
    $$PWD/editormanager/ieditorfactory.cpp \
    $$PWD/editormanager/iexternaleditor.cpp \
    $$PWD/actionmanager/actionmanager.cpp \
    $$PWD/actionmanager/command.cpp \
    $$PWD/actionmanager/commandbutton.cpp \
    $$PWD/actionmanager/actioncontainer.cpp \
    $$PWD/actionmanager/commandsfile.cpp \
    $$PWD/dialogs/saveitemsdialog.cpp \
    $$PWD/dialogs/newdialog.cpp \
    $$PWD/dialogs/settingsdialog.cpp \
    $$PWD/actionmanager/commandmappings.cpp \
    $$PWD/dialogs/shortcutsettings.cpp \
    $$PWD/dialogs/readonlyfilesdialog.cpp \
    $$PWD/dialogs/openwithdialog.cpp \
    $$PWD/progressmanager/progressmanager.cpp \
    $$PWD/progressmanager/progressview.cpp \
    $$PWD/progressmanager/progressbar.cpp \
    $$PWD/progressmanager/futureprogress.cpp \
    $$PWD/reaper.cpp \
    $$PWD/coreplugin.cpp \
    $$PWD/modemanager.cpp \
    $$PWD/basefilewizard.cpp \
    $$PWD/basefilewizardfactory.cpp \
    $$PWD/generatedfile.cpp \
    $$PWD/plugindialog.cpp \
    $$PWD/inavigationwidgetfactory.cpp \
    $$PWD/navigationwidget.cpp \
    $$PWD/manhattanstyle.cpp \
    $$PWD/minisplitter.cpp \
    $$PWD/styleanimator.cpp \
    $$PWD/findplaceholder.cpp \
    $$PWD/rightpane.cpp \
    $$PWD/sidebar.cpp \
    $$PWD/fileiconprovider.cpp \
    $$PWD/icore.cpp \
    $$PWD/infobar.cpp \
    $$PWD/editormanager/ieditor.cpp \
    $$PWD/dialogs/ioptionspage.cpp \
    $$PWD/settingsdatabase.cpp \
    $$PWD/imode.cpp \
    $$PWD/editormanager/systemeditor.cpp \
    $$PWD/designmode.cpp \
    $$PWD/editortoolbar.cpp \
    $$PWD/helpmanager.cpp \
    $$PWD/outputpanemanager.cpp \
    $$PWD/navigationsubwidget.cpp \
    $$PWD/sidebarwidget.cpp \
    $$PWD/externaltool.cpp \
    $$PWD/dialogs/externaltoolconfig.cpp \
    $$PWD/dialogs/filepropertiesdialog.cpp \
    $$PWD/toolsettings.cpp \
    $$PWD/variablechooser.cpp \
    $$PWD/mimetypemagicdialog.cpp \
    $$PWD/mimetypesettings.cpp \
    $$PWD/dialogs/promptoverwritedialog.cpp \
    $$PWD/fileutils.cpp \
    $$PWD/featureprovider.cpp \
    $$PWD/idocument.cpp \
    $$PWD/idocumentfactory.cpp \
    $$PWD/textdocument.cpp \
    $$PWD/documentmanager.cpp \
    $$PWD/iversioncontrol.cpp \
    $$PWD/dialogs/addtovcsdialog.cpp \
    $$PWD/ioutputpane.cpp \
    $$PWD/patchtool.cpp \
    $$PWD/windowsupport.cpp \
    $$PWD/opendocumentstreeview.cpp \
    $$PWD/messagebox.cpp \
    $$PWD/iwelcomepage.cpp \
    $$PWD/externaltoolmanager.cpp \
    $$PWD/systemsettings.cpp \
    $$PWD/coreicons.cpp \
    $$PWD/diffservice.cpp \
    $$PWD/menubarfilter.cpp

HEADERS += \
    $$PWD/corejsextensions.h \
    $$PWD/mainwindow.h \
    $$PWD/shellcommand.h \
    $$PWD/editmode.h \
    $$PWD/iwizardfactory.h \
    $$PWD/fancyactionbar.h \
    $$PWD/fancytabwidget.h \
    $$PWD/generalsettings.h \
    $$PWD/themechooser.h \
    $$PWD/id.h \
    $$PWD/jsexpander.h \
    $$PWD/messagemanager.h \
    $$PWD/messageoutputwindow.h \
    $$PWD/outputpane.h \
    $$PWD/outputwindow.h \
    $$PWD/vcsmanager.h \
    $$PWD/statusbarmanager.h \
    $$PWD/editormanager/editorarea.h \
    $$PWD/editormanager/editormanager.h \
    $$PWD/editormanager/editormanager_p.h \
    $$PWD/editormanager/editorview.h \
    $$PWD/editormanager/editorwindow.h \
    $$PWD/editormanager/documentmodel.h \
    $$PWD/editormanager/openeditorsview.h \
    $$PWD/editormanager/openeditorswindow.h \
    $$PWD/editormanager/ieditor.h \
    $$PWD/editormanager/iexternaleditor.h \
    $$PWD/editormanager/ieditorfactory.h \
    $$PWD/actionmanager/actioncontainer.h \
    $$PWD/actionmanager/actionmanager.h \
    $$PWD/actionmanager/command.h \
    $$PWD/actionmanager/commandbutton.h \
    $$PWD/actionmanager/actionmanager_p.h \
    $$PWD/actionmanager/command_p.h \
    $$PWD/actionmanager/actioncontainer_p.h \
    $$PWD/actionmanager/commandsfile.h \
    $$PWD/dialogs/saveitemsdialog.h \
    $$PWD/dialogs/newdialog.h \
    $$PWD/dialogs/settingsdialog.h \
    $$PWD/actionmanager/commandmappings.h \
    $$PWD/dialogs/readonlyfilesdialog.h \
    $$PWD/dialogs/shortcutsettings.h \
    $$PWD/dialogs/openwithdialog.h \
    $$PWD/dialogs/ioptionspage.h \
    $$PWD/progressmanager/progressmanager_p.h \
    $$PWD/progressmanager/progressview.h \
    $$PWD/progressmanager/progressbar.h \
    $$PWD/progressmanager/futureprogress.h \
    $$PWD/progressmanager/progressmanager.h \
    $$PWD/reaper.h \
    $$PWD/reaper_p.h \
    $$PWD/icontext.h \
    $$PWD/icore.h \
    $$PWD/infobar.h \
    $$PWD/imode.h \
    $$PWD/ioutputpane.h \
    $$PWD/coreconstants.h \
    $$PWD/iversioncontrol.h \
    $$PWD/ifilewizardextension.h \
    $$PWD/versiondialog.h \
    $$PWD/core_global.h \
    $$PWD/coreplugin.h \
    $$PWD/modemanager.h \
    $$PWD/basefilewizard.h \
    $$PWD/basefilewizardfactory.h \
    $$PWD/generatedfile.h \
    $$PWD/plugindialog.h \
    $$PWD/inavigationwidgetfactory.h \
    $$PWD/navigationwidget.h \
    $$PWD/manhattanstyle.h \
    $$PWD/minisplitter.h \
    $$PWD/styleanimator.h \
    $$PWD/findplaceholder.h \
    $$PWD/rightpane.h \
    $$PWD/sidebar.h \
    $$PWD/fileiconprovider.h \
    $$PWD/settingsdatabase.h \
    $$PWD/editormanager/systemeditor.h \
    $$PWD/designmode.h \
    $$PWD/editortoolbar.h \
    $$PWD/helpmanager.h \
    $$PWD/helpmanager_implementation.h \
    $$PWD/outputpanemanager.h \
    $$PWD/navigationsubwidget.h \
    $$PWD/sidebarwidget.h \
    $$PWD/externaltool.h \
    $$PWD/dialogs/externaltoolconfig.h \
    $$PWD/dialogs/filepropertiesdialog.h \
    $$PWD/toolsettings.h \
    $$PWD/variablechooser.h \
    $$PWD/mimetypemagicdialog.h \
    $$PWD/mimetypesettings.h \
    $$PWD/dialogs/promptoverwritedialog.h \
    $$PWD/fileutils.h \
    $$PWD/externaltoolmanager.h \
    $$PWD/generatedfile.h \
    $$PWD/featureprovider.h \
    $$PWD/idocument.h \
    $$PWD/idocumentfactory.h \
    $$PWD/textdocument.h \
    $$PWD/documentmanager.h \
    $$PWD/dialogs/addtovcsdialog.h \
    $$PWD/patchtool.h \
    $$PWD/windowsupport.h \
    $$PWD/opendocumentstreeview.h \
    $$PWD/messagebox.h \
    $$PWD/iwelcomepage.h \
    $$PWD/systemsettings.h \
    $$PWD/coreicons.h \
    $$PWD/editormanager/documentmodel_p.h \
    $$PWD/diffservice.h \
    $$PWD/menubarfilter.h \
    $$PWD/editormanager/ieditorfactory_p.h

FORMS += \
    $$PWD/dialogs/newdialog.ui \
    $$PWD/dialogs/saveitemsdialog.ui \
    $$PWD/dialogs/readonlyfilesdialog.ui \
    $$PWD/dialogs/openwithdialog.ui \
    $$PWD/generalsettings.ui \
    $$PWD/dialogs/externaltoolconfig.ui \
    $$PWD/dialogs/filepropertiesdialog.ui \
    $$PWD/mimetypesettingspage.ui \
    $$PWD/mimetypemagicdialog.ui \
    $$PWD/dialogs/addtovcsdialog.ui \
    $$PWD/systemsettings.ui

RESOURCES += \
    $$PWD/core.qrc \
    $$PWD/fancyactionbar.qrc

include($$PWD/find/find.pri)
include($$PWD/locator/locator.pri)

win32 {
    SOURCES += $$PWD/progressmanager/progressmanager_win.cpp
    QT += gui-private # Uses QPlatformNativeInterface.
    LIBS += -lole32 -luser32
}
else:macx {
    OBJECTIVE_SOURCES += \
        $$PWD/progressmanager/progressmanager_mac.mm
    LIBS += -framework AppKit
}
else:unix {
    SOURCES += $$PWD/progressmanager/progressmanager_x11.cpp

    IMAGE_SIZE_LIST = 16 24 32 48 64 128 256 512

    for(imagesize, IMAGE_SIZE_LIST) {
        eval(image$${imagesize}.files = $$PWD/images/logo/$${imagesize}/QtProject-qtcreator.png)
        eval(image$${imagesize}.path = $$QTC_PREFIX/share/icons/hicolor/$${imagesize}x$${imagesize}/apps)
        INSTALLS += image$${imagesize}
    }
}

equals(TEST, 1) {
    SOURCES += $$PWD/testdatadir.cpp
    HEADERS += $$PWD/testdatadir.h
}

DISTFILES += \
    $$PWD/Core.json.in

QMAKE_SUBSTITUTES += \
    $$PWD/Core.json.in
