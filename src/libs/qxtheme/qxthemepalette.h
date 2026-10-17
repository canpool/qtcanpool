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
 * Palette matching \a dark.
 *
 * The engine asks this for any registered theme that did not bring a palette of
 * its own, which is the common case: a theme says whether it is dark, and that
 * alone names the palette.
 */
QX_THEME_EXPORT QPalette paletteFor(bool dark);

QX_THEME_END_NAMESPACE

#endif   // QXTEMEPALETTE_H
