/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxringpainter.h"

#include <QtGui/QPainter>
#include <QtGui/QPainterPath>

QX_CONTROLS_BEGIN_NAMESPACE

namespace QxRingPainter
{

namespace
{
// A rounded rectangle deflated by \a inset, with corners that follow \a radius
// in by the same amount - clamped, because a deep inset on a small rectangle
// meets itself in the middle. The negative inset widens, which is how a ring
// hung over the edge is drawn.
QPainterPath rounded(const QRectF &rect, qreal inset, qreal radius)
{
    const QRectF adjusted = rect.adjusted(inset, inset, -inset, -inset);
    const qreal half = qMin(adjusted.width(), adjusted.height()) / 2.0;
    QPainterPath path;
    path.addRoundedRect(adjusted, qBound(0.0, radius - inset, half), qBound(0.0, radius - inset, half));
    return path;
}
}

void paint(QPainter *painter, const QRectF &rect, qreal cornerRadius, const QxRing &ring)
{
    if (!painter || !ring.color.isValid() || ring.width <= 0.0) {
        return;
    }
    const QPainterPath band = rounded(rect, ring.inset, cornerRadius);
    const QPainterPath hole = rounded(rect, ring.inset + ring.width, cornerRadius);

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);
    // The band as a fill with its inside taken out rather than a stroke along
    // its middle: a stroke of width w puts half of itself outside the band,
    // and on a corner that half disagrees with the rectangle it belongs to.
    painter->fillPath(band.subtracted(hole), ring.color);
    painter->restore();
}

}   // namespace QxRingPainter

QX_CONTROLS_END_NAMESPACE
