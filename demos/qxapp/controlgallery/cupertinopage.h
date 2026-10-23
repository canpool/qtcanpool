/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef CUPERTINOPAGE_H
#define CUPERTINOPAGE_H

#include "gallerypage.h"

/*!
 * The Cupertino half of the gallery: one row for every control QxCupertino
 * ships.
 *
 * One row per control, and there are four of them - a page that showed three
 * would be a page whose golden stopped covering the fourth the day it was
 * added. The rows are grouped by what a control is for, the way the other
 * pages are: a control that picks a value and a control that starts an action
 * are not the same question asked twice.
 *
 * Only this family's controls are on the page. Mixing two families in one page
 * would be deciding for the reader that they can be mixed, and nothing has been
 * done to align two families' shapes with each other.
 */
class CupertinoPage : public GalleryPage
{
    Q_OBJECT
public:
    explicit CupertinoPage(QWidget *parent = Q_NULLPTR);
};

#endif   // CUPERTINOPAGE_H
