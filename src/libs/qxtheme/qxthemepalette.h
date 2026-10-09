/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXTEMEPALETTE_H
#define QXTEMEPALETTE_H

#include "qxtheme.h"

#include <QtGui/QPalette>

QX_THEME_BEGIN_NAMESPACE

/*!
 * The palette that backs the light themes.
 *
 * The style sheets only reach the widgets they name explicitly; menus, native
 * dialogs, tooltips and disabled text are still painted from the application
 * palette. These palettes keep those surfaces consistent with the style sheets
 * instead of leaving them on the platform default.
 */
QX_THEME_EXPORT QPalette lightPalette();
/*! The palette that backs the dark themes. */
QX_THEME_EXPORT QPalette darkPalette();
/*!
 * Palette matching \a theme: the dark palette for DarkWps / DarkOfficePlus,
 * the light one otherwise.
 */
QX_THEME_EXPORT QPalette palette(Theme theme);

QX_THEME_END_NAMESPACE

#endif   // QXTEMEPALETTE_H
