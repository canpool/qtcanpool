/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxthemebundle.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

QX_APP_BEGIN_NAMESPACE

namespace
{
/*!
 * The one value the sheets cannot publish, for every bundled theme: the sheet
 * has its accent substituted in, where nothing can read it back, and what needs
 * it is the palette - a glyph on an emphasized surface is painted in the pair of
 * QPalette::Highlight, and only the theme knows what that fill is.
 *
 * Read once: the table is one small file and every bundled theme is registered
 * from it.
 */
const QJsonObject &bundledAccents()
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
}   // namespace

QxTheme::QxThemeInfo readBundledTheme(const QString &id, const QString &name, bool dark, const QString &fileName)
{
    QFile file(QStringLiteral(":/themes/") + fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QxTheme::QxThemeInfo();
    }

    QxTheme::QxThemeInfo info(id, name, dark);
    info.styleSheet = QString::fromUtf8(file.readAll());

    const QColor accent(bundledAccents().value(id).toString());
    if (accent.isValid()) {
        info.accent = accent;
    }
    return info;
}

void registerBundledThemes(QxTheme::QxThemeManager *engine)
{
    if (!engine) {
        return;
    }

    // One entry per bundled theme, read from
    // :/themes/<family>_<light|dark>.css. The ids and the file names follow
    // the canonical set in res/themes.qrc, so an application that ships those
    // files gets the whole bundle from this one call.
    struct Entry {
        const char *id;
        const char *name;
        bool dark;
        const char *file;
    };
    static const Entry entries[] = {
        {"soft-light", "Soft Light", false, "soft_light.css"},
        {"soft-dark", "Soft Dark", true, "soft_dark.css"},
        {"classic-light", "Classic Light", false, "classic_light.css"},
        {"classic-dark", "Classic Dark", true, "classic_dark.css"},
        {"flat-light", "Flat Light", false, "flat_light.css"},
        {"flat-dark", "Flat Dark", true, "flat_dark.css"},
        {"slate-light", "Slate Light", false, "slate_light.css"},
        {"slate-dark", "Slate Dark", true, "slate_dark.css"},
        {"accent-light", "Accent Light", false, "accent_light.css"},
        {"accent-dark", "Accent Dark", true, "accent_dark.css"},
    };

    for (const Entry &entry : entries) {
        engine->registerTheme(readBundledTheme(QString::fromUtf8(entry.id), QString::fromUtf8(entry.name), entry.dark,
                                               QString::fromUtf8(entry.file)));
    }
}

QX_APP_END_NAMESPACE
