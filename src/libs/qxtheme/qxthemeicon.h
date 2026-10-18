/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXTHEMEICON_H
#define QXTHEMEICON_H

#include "qxtheme.h"

#include <QtGui/QIcon>
#include <QtGui/QPalette>
#include <QtGui/QPixmap>

QX_THEME_BEGIN_NAMESPACE

/*!
 * Single-colour icons, painted with the palette the theme installed.
 *
 * A glyph shipped in one colour has to be repainted when the theme changes:
 * black on a light surface is unreadable on a dark one. These four functions are
 * the entry point an application - and QxApp, which is the only library that
 * sees the whole picture - asks for that; they read the colour from the palette
 * currently installed on QGuiApplication, which is the one QxThemeManager put
 * there.
 *
 * The icon is rasterised and then repainted through its alpha only, so what is
 * painted over is the shape and nothing else: the colour the file was drawn in
 * is irrelevant here, and a glyph with more than one colour comes out as a
 * silhouette.
 *
 * QxRibbon and QxDock each carry a routine of their own for the same job, and
 * neither of them is going anywhere: both libraries are usable without this one,
 * and making them call into qxtheme would turn two independent libraries into
 * two that depend on a third. What keeps the three honest is not shared code but
 * a shared rule - what a tintable icon may look like, and a gate that checks it.
 * See doc/pages/icons.md.
 */

/*!
 * \a filePath rasterised at \a size and repainted in \a color; a null pixmap
 * when the file cannot be loaded, and the untouched raster when \a color is
 * invalid - without an application there is no palette to ask, and a glyph in
 * its own colour is still a glyph.
 *
 * An empty \a size asks for the asset's own size.
 */
QX_THEME_EXPORT QPixmap tintedPixmap(const QString &filePath, const QColor &color, const QSize &size = QSize());
/*!
 * tintedPixmap() at several sizes, as one icon.
 *
 * Each size is rasterised separately, which is what keeps a vector glyph sharp:
 * one raster scaled up is blurry, a raster made at the size it is drawn at is
 * not. An empty \a size asks for the asset's own size, scaled for the common
 * device pixel ratios.
 */
QX_THEME_EXPORT QIcon tintedIcon(const QString &filePath, const QColor &color, const QSize &size = QSize());

/*!
 * tintedPixmap() in the colour \a role names in the palette currently installed.
 *
 * Asking the palette rather than naming a colour is the whole point: the same
 * call answers differently on a light theme and a dark one, and an application
 * that hands its icons through this function needs no code of its own to follow
 * a theme change beyond re-asking.
 */
QX_THEME_EXPORT QPixmap themedPixmap(const QString &filePath, QPalette::ColorRole role = QPalette::WindowText,
                                     const QSize &size = QSize());
/*! themedPixmap() at several sizes, as one icon. */
QX_THEME_EXPORT QIcon themedIcon(const QString &filePath, QPalette::ColorRole role = QPalette::WindowText,
                                 const QSize &size = QSize());

QX_THEME_END_NAMESPACE

#endif   // QXTHEMEICON_H
