/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALPAGE_H
#define MATERIALPAGE_H

#include "gallerypage.h"

/*!
 * The Material half of the gallery: one row for every control QxMaterial ships.
 *
 * The two pages are kept in the same shape on purpose - same sections, same
 * order of answers - because the question the gallery exists to answer is a
 * comparison: the same job done two ways, side by side in the reader's head.
 * Where one set has no answer to a section's question, the section is simply
 * shorter, and that too is part of the answer.
 */
class MaterialPage : public GalleryPage
{
    Q_OBJECT
public:
    explicit MaterialPage(QWidget *parent = Q_NULLPTR);
};

#endif   // MATERIALPAGE_H
