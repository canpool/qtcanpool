/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXTHEME_H
#define QXTHEME_H

#include "qxtheme_global.h"

#include <QtCore/QMetaType>
#include <QtCore/QString>

QX_THEME_BEGIN_NAMESPACE

/*!
 * The themes understood by QxThemeManager.
 *
 * The order mirrors QX_RIBBON_PREPEND_NAMESPACE(RibbonTheme)::ThemeStyle, so the
 * two enumerations map onto each other without a translation table. Custom
 * means "ship your own style sheet" and keeps the built-in resources untouched.
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

QX_THEME_END_NAMESPACE

Q_DECLARE_METATYPE(QX_THEME_PREPEND_NAMESPACE(Theme))

#endif   // QXTHEME_H
