/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef HARMONYPAGE_H
#define HARMONYPAGE_H

#include "gallerypage.h"

/*!
 * The Harmony half of the gallery: one row for every control QxHarmony ships.
 *
 * One row per control, and there are four of them, in the same shape as the
 * other pages: same idea of a section, same caption column, nothing arranged
 * for effect. What the four are, though, is this family's answer and not a
 * translation of another page's: a switch, a slider, a card and a button.
 *
 * Only this family's controls are on the page. Mixing two families in one page
 * would be deciding for the reader that they can be mixed, and nothing has been
 * done to align one family's shapes with another's.
 */
class HarmonyPage : public GalleryPage
{
    Q_OBJECT
public:
    explicit HarmonyPage(QWidget *parent = Q_NULLPTR);
};

#endif   // HARMONYPAGE_H
