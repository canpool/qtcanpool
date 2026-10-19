/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "themestart.h"

#include "qxtheme/qxtheme.h"
#include "qxtheme/qxthememanager.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QStringList>

#ifdef QTC_EXAMPLE_DOCK
#include "qxdock/dockstylesheet.h"
QX_DOCK_USE_NAMESPACE
#endif

QX_THEME_USE_NAMESPACE

namespace
{
/*!
 * The theme a sample wears when nobody asked for one and nothing was restored.
 * The dock demo starts on the same one, so the two agree on what the libraries
 * look like out of the box.
 */
const char kDefaultTheme[] = "soft-light";

/*! One sheet in res/themes.qrc, named the way the engine wants it named. */
struct Bundled
{
    QString id;    /*!< "soft-light" - what goes into the configuration. */
    QString name;  /*!< "Soft Light" - what a settings dialog would show. */
    bool dark;     /*!< Whether it draws on a dark background. */
    QString file;  /*!< "soft_light.css" - where in :/themes it lives. */
};

/*! "soft_light" -> "Soft Light". */
QString displayName(const QString &stem)
{
    QString name;
    bool capitalise = true;
    for (const QChar &ch : stem) {
        name += ch == QLatin1Char('_') ? QLatin1Char(' ')
                                       : (capitalise ? ch.toUpper() : ch);
        capitalise = (ch == QLatin1Char('_'));
    }
    return name;
}

/*!
 * Every sheet in :/themes, in the order the directory lists them.
 *
 * The id and the name are derived from the file name, which is the convention
 * res/themes.qrc and the generator both follow: <family>_<scheme>.css. Reading
 * the directory instead of repeating the list keeps this file true when a
 * family is added - the sample offers the new one without being told about it.
 */
QList<Bundled> bundledSheets()
{
    QList<Bundled> sheets;
    const QDir dir(QStringLiteral(":/themes"));
    const QStringList files = dir.entryList(QStringList(QStringLiteral("*.css")), QDir::Files, QDir::Name);
    for (const QString &file : files) {
        const QString stem = QFileInfo(file).completeBaseName();
        const int split = stem.lastIndexOf(QLatin1Char('_'));
        if (split <= 0) {
            continue;   // Not <family>_<scheme>, so not a theme sheet.
        }
        Bundled sheet;
        sheet.file = file;
        sheet.id = QString(stem).replace(QLatin1Char('_'), QLatin1Char('-'));
        sheet.name = displayName(stem);
        sheet.dark = stem.mid(split + 1) == QStringLiteral("dark");
        sheets.append(sheet);
    }
    return sheets;
}

/*!
 * The one value a sheet cannot publish, for every bundled theme: the accent was
 * substituted into the sheet where nothing can read it back, and what needs it
 * is the palette - a glyph on an emphasized surface is painted in the pair of
 * QPalette::Highlight, and only the theme knows what that fill is.
 */
QJsonObject bundledAccents()
{
    static const QJsonObject table = [] {
        QFile file(QStringLiteral(":/themes/accents.json"));
        if (!file.open(QIODevice::ReadOnly)) {
            return QJsonObject();
        }
        return QJsonDocument::fromJson(file.readAll()).object();
    }();
    return table;
}

/*! Registers every sheet in res/themes.qrc with \a engine. */
void registerBundled(QxThemeManager *engine)
{
    const QJsonObject accents = bundledAccents();
    for (const Bundled &sheet : bundledSheets()) {
        QFile file(QStringLiteral(":/themes/") + sheet.file);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            continue;
        }
        QxThemeInfo info(sheet.id, sheet.name, sheet.dark);
        info.styleSheet = QString::fromUtf8(file.readAll());
#ifdef QTC_EXAMPLE_DOCK
        // The dock's own sheet is not installed beside the theme's - it would be
        // a second copy of the same rules - but the platform's piece of it is
        // the one part a theme does not carry, so it is appended here rather
        // than lost. Empty where the platform needs nothing.
        info.styleSheet += dockPlatformStyleSheet();
#endif
        // The pair the sheet was drawn with, not one colour and a computation:
        // what reads on the accent is a choice the theme already made.
        const QJsonObject entry = accents.value(sheet.id).toObject();
        const QColor accent(entry.value(QStringLiteral("accent")).toString());
        if (accent.isValid()) {
            info.accent = accent;
        }
        const QColor onAccent(entry.value(QStringLiteral("on-accent")).toString());
        if (onAccent.isValid()) {
            info.onAccent = onAccent;
        }
        engine->registerTheme(info);
    }
}
}   // namespace

bool installExampleTheme()
{
    const QStringList arguments = QCoreApplication::arguments();
    QString requested;
    for (int i = 1; i < arguments.size(); ++i) {
        if (arguments.at(i) == QStringLiteral("--theme") && i + 1 < arguments.size()) {
            requested = arguments.value(++i);
        }
    }

    QxThemeManager *engine = QxThemeManager::instance();
    registerBundled(engine);

    if (!requested.isEmpty()) {
        if (!engine->hasTheme(requested)) {
            qWarning("Example: no bundled theme '%s'. Available: %s", qPrintable(requested),
                     qPrintable(engine->themeIds().join(QStringLiteral(", "))));
            return false;
        }
        engine->setTheme(requested);
        return true;
    }

    // A selection restored from the configuration was installed by the
    // registration above, at the moment the theme it names arrived. Without one
    // there is nothing to wear, and a sample that left the palette to Qt is
    // showing the platform rather than the library.
    if (!engine->hasTheme(engine->theme())) {
        engine->setTheme(QString::fromLatin1(kDefaultTheme));
    }
    return true;
}
