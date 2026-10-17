/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxthemebundle.h"

#include <QtCore/QFile>

QX_APP_BEGIN_NAMESPACE

QxTheme::QxThemeInfo readBundledTheme(const QString &id, const QString &name, bool dark, const QString &fileName)
{
    QFile file(QStringLiteral(":/themes/") + fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QxTheme::QxThemeInfo();
    }

    QxTheme::QxThemeInfo info(id, name, dark);
    info.styleSheet = QString::fromUtf8(file.readAll());
    return info;
}

void registerBundledThemes(QxTheme::QxThemeManager *engine)
{
    if (!engine) {
        return;
    }

    // Five families, each in a light and a dark scheme, read from
    // :/themes/<family>_<light|dark>.css. The ids and the file names follow
    // the canonical set in res/themes.qrc; an application that ships those
    // files gets all ten themes from this one call.
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
