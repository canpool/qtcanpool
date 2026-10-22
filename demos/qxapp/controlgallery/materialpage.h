/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALPAGE_H
#define MATERIALPAGE_H

#include "gallerypage.h"

#include "qxmaterial/qxmaterial_global.h"

QX_MATERIAL_BEGIN_NAMESPACE
class MaterialDialog;
QX_MATERIAL_END_NAMESPACE

QX_MATERIAL_USE_NAMESPACE

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
private:
    /*!
     * The dialog, made the first time it is asked for. It is a window that
     * covers the page it was opened over, so it can only exist while it is
     * open - and one that existed before it was wanted would be a second
     * top-level window the capture has to explain.
     */
    MaterialDialog *m_dialog = Q_NULLPTR;
};

#endif   // MATERIALPAGE_H
