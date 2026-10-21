/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXSHADOWPAINTER_H
#define QXSHADOWPAINTER_H

#include "qxcontrols_global.h"

#include "qxtheme/qxthemeelevation.h"

#include <QtGui/QColor>

class QPainter;
class QRectF;

QX_CONTROLS_BEGIN_NAMESPACE

/*!
 * The painted shadow: the mechanism that turns a tier from
 * qxthemeelevation.h into pixels.
 *
 * QSS cannot draw a shadow and a QGraphicsDropShadowEffect is one effect per
 * widget - too costly to hand out and awkward with QWidget::grab(). So a
 * shadow is drawn the way a border is: a small tile is rendered once for a
 * set of parameters - the tier's blur, its alpha, the radius of the corners
 * it belongs to, the colour it paints with - and the tile is stretched over
 * the rectangle in nine pieces, corners whole and edges stretched. The tile
 * is cached, so the second card with the same shape and the same tier costs
 * a lookup, not a render.
 *
 * The blur is an approximation by concentric rings rather than a true
 * gaussian - a dozen rings read as one smooth falloff at the sizes a shadow
 * is drawn at, and the rings keep the render deterministic and cheap. Call
 * from the GUI thread; that is where painting happens anyway.
 */
namespace QxShadowPainter
{

/*!
 * Draws the shadow of a rounded rectangle: \a color at \a shadow's alpha,
 * spreading \a shadow's blur beyond \a rect and sitting \a shadow's y offset
 * below it. The control draws its own fill afterwards; the shadow is what is
 * under it.
 *
 * A shadow with no alpha, no blur or an invalid colour draws nothing - the
 * flat tier of the vocabulary, spelled as "do not draw one". The painter's
 * state is the caller's; anti-aliasing is switched on for the drawing and
 * left the way it was found.
 */
void QX_CONTROLS_EXPORT paint(QPainter *painter, const QRectF &rect, qreal cornerRadius, const QColor &color,
                              const QxTheme::QxShadow &shadow);

/*!
 * The tile \a paint stretches, rendered for the parameters named - for a test
 * that wants to predict a pixel, and for paint() itself. The parameters are
 * quantized to whole pixels: the cache would not survive a fractional radius.
 */
QPixmap QX_CONTROLS_EXPORT tile(qreal cornerRadius, const QColor &color, const QxTheme::QxShadow &shadow);

}   // namespace QxShadowPainter

QX_CONTROLS_END_NAMESPACE

#endif   // QXSHADOWPAINTER_H
