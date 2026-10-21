/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXRINGPAINTER_H
#define QXRINGPAINTER_H

#include "qxcontrols_global.h"

#include <QtGui/QColor>

class QPainter;
class QRectF;

QX_CONTROLS_BEGIN_NAMESPACE

/*!
 * One ring of the focus indication: a colour, a thickness, and how far inside
 * the control's rectangle it sits.
 *
 * A ring is a band, not a stroke around a line: it occupies the strip from \a
 * inset to \a inset + \a width inside the rectangle it is painted on, so a
 * ring never falls outside the control and never has to be clipped. A
 * negative \a inset is how the languages that hang the ring over the edge
 * (Fluent's outer ring reaches past the control) say "outside".
 */
struct QxRing
{
    QColor color;   /*!< The colour the band paints with. */
    qreal width;    /*!< Band thickness in device independent pixels. */
    qreal inset;    /*!< Distance from the rectangle's edge to the band's outer edge. */
};

/*!
 * The focus ring: how "the keyboard is here" is drawn.
 *
 * Both design languages draw it as a band around the control and differ in
 * how many bands and what colours - Fluent pairs an outer contrast ring with
 * an inner accent ring, Material paints one accent band just off the edge.
 * The bands are the family's answer; painting one band, without seams at the
 * corners and without letting it fall off the rectangle, is mechanism, and
 * mechanism is here. A control that wants Fluent's pair calls twice.
 */
namespace QxRingPainter {

/*!
 * Draws one \a ring around \a rect, whose corners turn with \a cornerRadius -
 * the radius of the control the ring belongs to, not of the band: the band's
 * own path follows the corner in by \a inset + half the width, so a ring that
 * hugs the edge has the same shape as the control it belongs to.
 *
 * Anti-aliasing is switched on for the drawing and left the way it was found;
 * everything else about the painter's state is the caller's. A ring with no
 * colour or no width draws nothing, which is a control that shows no focus
 * and not an error.
 */
void QX_CONTROLS_EXPORT paint(QPainter *painter, const QRectF &rect, qreal cornerRadius, const QxRing &ring);

}   // namespace QxRingPainter

QX_CONTROLS_END_NAMESPACE

#endif   // QXRINGPAINTER_H
