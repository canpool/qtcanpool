/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef ICONS_H
#define ICONS_H

#include <QtCore/QString>
#include <QtGui/QColor>
#include <QtGui/QIcon>

/*!
 * The glyphs the demos put beside their names.
 *
 * One set of files, kept once. The ribbon demo's groups, the fluent demo's pane
 * and the material demo's rail and app bar all draw from the same svg
 * directory, so a glyph is added or replaced in one place and the three demos
 * follow. The files are Tabler Icons 3.49.0: monochrome stroke glyphs, written
 * in the canonical colour the icon policy asks for (see doc/pages/icons.md).
 *
 * A stroke glyph carries one colour in its file and cannot read a palette, so a
 * set that is used at all has to be painted in the colour of the page it is
 * drawn on - otherwise every icon is either invisible in one theme or wrong in
 * the other. The two demos that already had an answer keep it, and this layer
 * serves both: file() is for a consumer that paints the glyph from the palette
 * every time it is needed (QxTheme::themedIcon(), QxThemeIcons - the ribbon and
 * material demos), and named() is for one that keeps the picture and makes it
 * again when the theme moves (the fluent demo).
 */
namespace Icons
{
/*!
 * The file of \a name: one of the svgs under `svg/`, named without its
 * extension.
 */
QString file(const QString &name);

/*! \a name as a glyph in \a ink, sixteen pixels square.
 *
 *  Two glyphs of one name in two inks are two pictures: each is drawn once and
 *  kept, because a pane lists the same icon again on every keystroke of the
 *  caption's search. The size is the pane's, which is the one caller that keeps
 *  the picture; a consumer that needs another size paints the file itself.
 */
QIcon named(const QString &name, const QColor &ink);
}   // namespace Icons

#endif   // ICONS_H
