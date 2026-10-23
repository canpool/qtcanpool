/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTPAGE_H
#define FLUENTPAGE_H

#include "gallerypage.h"

#include "qxfluent/qxfluent_global.h"

QX_FLUENT_BEGIN_NAMESPACE
class FluentContentDialog;
class FluentTeachingTip;
class FluentWindowMaterial;
QX_FLUENT_END_NAMESPACE

class FluentMaterialWindow;

QX_FLUENT_USE_NAMESPACE

/*!
 * The Fluent half of the gallery: one row for every control QxFluent ships.
 *
 * The rows are grouped by what a control is for rather than by class, because
 * that is the order a reader looks for a control in - and because two controls
 * that answer the same question (an info bar and a snack bar, a switch and a
 * rating) belong next to each other even when they are painted by different
 * libraries.
 */
class FluentPage : public GalleryPage
{
    Q_OBJECT
public:
    explicit FluentPage(QWidget *parent = Q_NULLPTR);
private:
    /*!
     * The tip, made the first time it is asked for. A teaching tip is a popup
     * window of its own, and one that exists before it is wanted is a second
     * top-level window the capture has to explain.
     */
    FluentTeachingTip *m_tip = Q_NULLPTR;
    /*!
     * And the dialog, for the same reason: it is a window that covers the page
     * it was opened over, so it can only exist while it is open.
     */
    FluentContentDialog *m_dialog = Q_NULLPTR;
    /*!
     * Likewise made on the first ask: it is the window a material is shown on,
     * frameless and drawing its own caption - the only kind of window a
     * material can be asked of. The gallery's own window keeps its system
     * title bar and is never asked.
     */
    FluentMaterialWindow *m_materialWindow = Q_NULLPTR;
    /*!
     * The wrapper round qxwindow's materials, made with the window above -
     * a material belongs to one window.
     */
    FluentWindowMaterial *m_windowMaterial = Q_NULLPTR;
};

#endif   // FLUENTPAGE_H
