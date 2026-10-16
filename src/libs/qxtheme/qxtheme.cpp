/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxtheme.h"

#include <QtCore/QFile>
#include <QtGui/QGuiApplication>

#if defined(Q_OS_WIN) && QT_VERSION < QT_VERSION_CHECK(6, 6, 0)
// Only the Windows branch below needs these, and only below 6.6: from 6.6 on
// the colour arrives through QPalette::Accent and no system call is made.
#include <QtCore/qt_windows.h>
#endif

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

/*!
 * The Windows accent below Qt 6.6, kept as its own function so the version
 * branch reads as one decision rather than as a block in the middle of another.
 */
#if defined(Q_OS_WIN) && QT_VERSION < QT_VERSION_CHECK(6, 6, 0)
static QColor accentFromRegistry()
{
    HKEY key = Q_NULLPTR;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, LR"(Software\Microsoft\Windows\DWM)", 0, KEY_READ, &key) != ERROR_SUCCESS) {
        return QColor();
    }

    DWORD value = 0;
    DWORD size = sizeof(value);
    DWORD type = 0;
    const LONG status =
        RegQueryValueExW(key, L"AccentColor", Q_NULLPTR, &type, reinterpret_cast<LPBYTE>(&value), &size);
    RegCloseKey(key);
    if (status != ERROR_SUCCESS || type != REG_DWORD || size != sizeof(value)) {
        return QColor();
    }

    // The key holds #AABBGGRR while QColor::fromRgba() reads #AARRGGBB, so the
    // three colour bytes come back in the opposite order; alpha is in its place
    // either way.
    const QColor swapped = QColor::fromRgba(value);
    return QColor::fromRgb(swapped.blue(), swapped.green(), swapped.red(), swapped.alpha());
}
#endif

QColor systemAccentColor()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
    return QGuiApplication::palette().color(QPalette::Accent);
#elif defined(Q_OS_WIN)
    return accentFromRegistry();
#else
    // The desktop's own accent is not readable through Qt below 6.6, and there
    // is no one place the various desktops agree on. Saying so is better than
    // inventing a colour here.
    return QColor();
#endif
}

QX_THEME_END_NAMESPACE
