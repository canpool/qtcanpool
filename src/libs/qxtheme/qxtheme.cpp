/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxtheme.h"

#include <QtCore/QFile>

QX_THEME_BEGIN_NAMESPACE

/*!
 * Where the built-in style sheets are looked up. They are shipped by the
 * qxribbon resource bundle; the prefix is overridable at build time so a
 * downstream product can relocate its own copies without patching the source.
 */
#ifndef QX_THEME_STYLESHEET_PREFIX
#define QX_THEME_STYLESHEET_PREFIX ":/qxribbon/res/stylesheets/"
#endif

namespace
{

struct ThemeInfo {
    const char *id;
    const char *name;
    const char *fileName;   // empty for Custom
    bool dark;
};

const ThemeInfo kThemeInfo[] = {
    {"light-yellow", "Light Yellow", "light_yellow.css", false},                // LightYellow
    {"light-office2013", "Light Office 2013", "light_office2013.css", false},   // LightOffice2013
    {"light-classic", "Light Classic", "light_classic.css", false},             // LightClassic
    {"light-fancy", "Light Fancy", "light_fancy.css", false},                   // LightFancy
    {"dark-wps", "Dark WPS", "dark_wps.css", true},                             // DarkWps
    {"dark-officeplus", "Dark Office Plus", "dark_officeplus.css", true},       // DarkOfficePlus
    {"custom", "Custom", "", false},                                            // Custom
};

const int kThemeCount = int(sizeof(kThemeInfo) / sizeof(kThemeInfo[0]));

/*! Guards against a Theme value that does not match the table. */
bool isValidTheme(Theme theme)
{
    const int index = int(theme);
    return index >= 0 && index < kThemeCount;
}

const ThemeInfo *infoOf(Theme theme)
{
    return isValidTheme(theme) ? &kThemeInfo[int(theme)] : Q_NULLPTR;
}

}   // namespace

QString themeId(Theme theme)
{
    const ThemeInfo *info = infoOf(theme);
    return info ? QString::fromLatin1(info->id) : QString();
}

QString themeName(Theme theme)
{
    const ThemeInfo *info = infoOf(theme);
    return info ? QString::fromLatin1(info->name) : QString();
}

Theme themeFromId(const QString &id, Theme fallback)
{
    for (int i = 0; i < kThemeCount; ++i) {
        if (id == QLatin1String(kThemeInfo[i].id)) {
            return Theme(i);
        }
    }
    return fallback;
}

bool isDarkTheme(Theme theme)
{
    const ThemeInfo *info = infoOf(theme);
    return info ? info->dark : false;
}

QString themeStyleSheetPath(Theme theme)
{
    const ThemeInfo *info = infoOf(theme);
    if (!info || info->fileName[0] == '\0') {
        return QString();
    }
    return QString::fromLatin1(QX_THEME_STYLESHEET_PREFIX) + QString::fromLatin1(info->fileName);
}

QString themeStyleSheet(Theme theme)
{
    const QString path = themeStyleSheetPath(theme);
    if (path.isEmpty()) {
        return QString();
    }
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }
    return QString::fromUtf8(file.readAll());
}

QX_THEME_END_NAMESPACE
