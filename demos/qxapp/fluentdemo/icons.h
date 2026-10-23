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
 * The glyphs this demo puts beside the names of its screens.
 *
 * A stroke glyph carries one colour in its file and cannot read a palette, so
 * a set that is used at all has to be painted in the colour of the page it is
 * drawn on - otherwise every icon is either invisible in one theme or wrong in
 * the other. Each name is asked for in an ink and comes back as a picture of
 * that ink, which is why the icons in this demo follow the theme the same way
 * its text does.
 */
namespace Icons
{
/*! \a name as a glyph in \a ink, one of the files under `res/icons`.
 *
 *  Two glyphs of one name in two inks are two pictures: each is drawn once and
 *  kept, because a pane lists the same icon again on every keystroke of the
 *  caption's search.
 */
QIcon named(const QString &name, const QColor &ink);
}   // namespace Icons

#endif   // ICONS_H
