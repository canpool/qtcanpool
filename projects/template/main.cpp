/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

// A minimal QtCanpool application.
//
// QxAppShell already provides what every application otherwise rebuilds by
// hand: the ribbon window, the navigation rail, the page stack, a dock area
// around it and a status bar. Replace the page below with your own widget and
// add more pages/docks - see qxapp/qxappshell.h for the full API.
//
// The three things an application is asked for on day one are wired here, so
// they can be seen working before they are replaced:
//
//   * switch language  - QxTranslator, driven by a settings entry
//   * notify           - QxAppShell::showToast()
//   * open settings    - QxSettingsDialog, fed by QxProperty descriptors
//
// Note that only public headers are included. There is no relative include into
// the QtCanpool source tree, which is the whole point of consuming the SDK.

#include "qxapp/qxappshell.h"
#include "qxapp/qxproperty.h"
#include "qxapp/qxsettingsdialog.h"

#include "qxcore/qxsettings.h"
#include "qxcore/qxtranslator.h"

#include "qxribbon/ribbongroup.h"
#include "qxribbon/ribbonpage.h"

// QAction moved from QtWidgets to QtGui in Qt 6, so it is included without a
// module prefix: this file has to compile against both.
#include <QAction>
#include <QtCore/QCoreApplication>
#include <QtCore/QLocale>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>

QX_APP_USE_NAMESPACE
QX_CORE_USE_NAMESPACE
QX_RIBBON_USE_NAMESPACE

namespace
{
/*! Where the two settings below are stored; a key is a path, not a label. */
const auto kLanguageKey = QStringLiteral("ui/language");
const auto kNotificationsKey = QStringLiteral("ui/notifications");
}   // namespace

int main(int argc, char *argv[])
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("qtcanpool"));
    QCoreApplication::setApplicationName(QStringLiteral("qxtemplate"));

    QxAppShell shell;
    shell.addPage(QStringLiteral("home"), QIcon(), QObject::tr("Home"), new QLabel(QObject::tr("Hello, QtCanpool!")));
    shell.setStatusMessage(QObject::tr("Ready"));

    QxSettings *settings = shell.settings();

    // Switching language: the translator loads "<name>_<language>.qm" out of the
    // directory it points at, and the choice is a setting like any other - read
    // once at start-up, written back whenever it changes.
    QxTranslator translator;
    translator.setTranslationPath(QCoreApplication::applicationDirPath());
    QObject::connect(&translator, &QxTranslator::languageChanged, settings, [settings](const QString &language) {
        settings->setValue(kLanguageKey, language);
    });
    translator.setLanguage(settings->stringValue(kLanguageKey, QLocale::system().name()));

    // The settings dialog is described rather than hand-built: every entry is a
    // QxProperty, and the page is the list of them.
    QxProperty language;
    language.key = kLanguageKey;
    language.label = QObject::tr("Language");
    language.type = QxProperty::Enum;
    language.choices = translator.availableLanguages();
    if (language.choices.isEmpty()) {
        // No catalogue deployed beside the executable: offer the one in use, so
        // the field is never empty.
        language.choices.append(QLocale::system().name());
    }
    language.defaultValue = QLocale::system().name();
    language.tooltip = QObject::tr("Takes effect as soon as the dialog is applied");

    QxProperty notifications;
    notifications.key = kNotificationsKey;
    notifications.label = QObject::tr("Show notifications");
    notifications.type = QxProperty::Bool;
    notifications.defaultValue = true;

    QxSettingsDialog *dialog = new QxSettingsDialog(settings, &shell);
    dialog->addPage(QStringLiteral("general"), QIcon(), QObject::tr("General"),
                    QList<QxProperty>{language, notifications});

    QObject::connect(dialog, &QxSettingsDialog::applied, &shell, [&]() {
        translator.setLanguage(settings->stringValue(kLanguageKey, QLocale::system().name()));
        if (settings->value(kNotificationsKey, true).toBool()) {
            shell.showToast(QObject::tr("Settings applied"), QxToast::Success);
        }
    });

    // The way into it: the shell is a ribbon window, so the bar is already there
    // and only needs a page, a group and an action.
    RibbonPage *home = shell.ribbonBar()->addPage(QObject::tr("Home"));
    RibbonGroup *application = home->addGroup(QObject::tr("Application"));
    QAction *settingsAction = new QAction(QObject::tr("Settings"), &shell);
    application->addLargeAction(settingsAction);
    QObject::connect(settingsAction, &QAction::triggered, dialog, [dialog]() {
        dialog->exec();
    });

    // Restore after every page and dock exists: the layout is keyed by page id,
    // so a page added later would have nothing to be restored into.
    shell.restoreLayout();

    shell.show();

    // Notifications: the setting decides whether they appear at all.
    if (settings->value(kNotificationsKey, true).toBool()) {
        shell.showToast(QObject::tr("Welcome to QtCanpool!"));
    }

    return app.exec();
}
