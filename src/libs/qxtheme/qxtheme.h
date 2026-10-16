/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXTHEME_H
#define QXTHEME_H

#include "qxtheme_global.h"

#include <QtCore/QMetaType>
#include <QtCore/QString>
#include <QtGui/QColor>

QX_THEME_BEGIN_NAMESPACE

/*!
 * The themes understood by QxThemeManager.
 *
 * The order mirrors the ThemeStyle enumeration of the ribbon theme, so the two
 * enumerations map onto each other without a translation table. Custom means
 * "ship your own style sheet" and keeps the built-in resources untouched.
 */
enum Theme {
    LightYellow,
    LightOffice2013,
    LightClassic,
    LightFancy,
    DarkWps,
    DarkOfficePlus,
    Custom,
};

/*! Stable identifier written to the configuration. Never translate it. */
QX_THEME_EXPORT QString themeId(Theme theme);
/*! Human readable name, suitable for a settings dialog. */
QX_THEME_EXPORT QString themeName(Theme theme);
/*! Resolves an id produced by themeId(); \a fallback is returned for unknown ids. */
QX_THEME_EXPORT Theme themeFromId(const QString &id, Theme fallback = LightYellow);
/*! True for the themes that draw on a dark background. */
QX_THEME_EXPORT bool isDarkTheme(Theme theme);
/*!
 * Resource path of the style sheet shipped with \a theme, empty for Custom.
 *
 * The style sheets live in the qxribbon resource bundle, so the path only
 * resolves when that library is linked into the process. Use
 * QxThemeManager::setStyleSheetFile() to point a theme somewhere else.
 */
QX_THEME_EXPORT QString themeStyleSheetPath(Theme theme);
/*! Content of the built-in style sheet, empty when the resource is missing. */
QX_THEME_EXPORT QString themeStyleSheet(Theme theme);

/*!
 * The accent colour the platform uses, or an invalid QColor when it does not
 * name one.
 *
 * From Qt 6.6 on this is QPalette::Accent, which the platform plugin fills in
 * from the system - the Windows accent, the desktop's selection colour - so
 * that path is one piece of code on every platform. Below 6.6 only Windows
 * keeps the colour somewhere readable, under the DWM key of the registry, and
 * that is the single platform-specific branch here; everywhere else the answer
 * is that the platform does not say.
 *
 * This is what the system offers, not what is in force: a theme of this library
 * carries its own highlight colour, and QxThemeManager::accentColor() is the
 * one to ask which of the two is actually being used.
 */
QX_THEME_EXPORT QColor systemAccentColor();

QX_THEME_END_NAMESPACE

Q_DECLARE_METATYPE(QX_THEME_PREPEND_NAMESPACE(Theme))

#endif   // QXTHEME_H
