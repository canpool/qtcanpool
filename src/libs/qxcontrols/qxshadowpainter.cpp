/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxshadowpainter.h"

#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtMath>
#include <QtCore/QCache>
#include <QtCore/qhashfunctions.h>

#include <cmath>

QX_CONTROLS_BEGIN_NAMESPACE

namespace QxShadowPainter
{

namespace
{
// How far the shadow reaches beyond the rectangle: the blur, plus a pixel for
// the anti-aliased edge to land on.
int spread(const QxTheme::QxShadow &shadow)
{
    return int(qCeil(shadow.blurRadius)) + 1;
}

// The side of the straight-edge region the tile renders in the middle: wide
// enough that the corners are finished before it starts, so stretching it is
// honest. The corner pieces are radius + spread on their own. It does not
// depend on the shadow: the spread is added around this, not inside it.
int coreSide(int radius)
{
    return 2 * radius + 16;
}

QPainterPath roundedPath(const QRectF &rect, qreal radius)
{
    QPainterPath path;
    // Clamped at zero, for the reason qxringpainter gives: a rectangle that
    // has met itself in the middle is not one a radius can be bounded by.
    const qreal half = qMax(0.0, qMin(rect.width(), rect.height()) / 2.0);
    path.addRoundedRect(rect, qBound(0.0, radius, half), qBound(0.0, radius, half));
    return path;
}

QPixmap renderTile(int radius, const QColor &color, const QxTheme::QxShadow &shadow)
{
    const int margin = spread(shadow);
    const int side = coreSide(radius);
    const int size = side + 2 * margin;
    QPixmap tile(size, size);
    tile.fill(Qt::transparent);

    QPainter painter(&tile);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);

    // The shadow falls off in rings: ring i covers the band between two
    // expansions of the rectangle, and the rings do not overlap, so their
    // alphas add to a falloff instead of to a stack. The tier's alpha is what
    // the shadow reads at its darkest - right under the control's edge.
    const QRectF core(margin, margin, side, side);
    QColor coreColor = color;
    coreColor.setAlphaF(color.alphaF() * shadow.alpha / 255.0);
    painter.fillPath(roundedPath(core, radius), coreColor);

    const int rings = 16;
    const qreal step = shadow.blurRadius / rings;
    for (int i = 0; i < rings; ++i) {
        const qreal inner = i * step;
        const qreal outer = (i + 1) * step;
        QColor ringColor = color;
        ringColor.setAlphaF(color.alphaF() * shadow.alpha / 255.0 * std::exp(-3.0 * inner / shadow.blurRadius));
        const QPainterPath out = roundedPath(core.adjusted(-outer, -outer, outer, outer), radius + outer);
        const QPainterPath in = roundedPath(core.adjusted(-inner, -inner, inner, inner), radius + inner);
        painter.fillPath(out.subtracted(in), ringColor);
    }
    painter.end();
    return tile;
}

QCache<qint64, QPixmap> &cache()
{
    // The handful of shapes a window draws is a handful of tiles; the cost cap
    // is a floor under that, not a tuning knob.
    static QCache<qint64, QPixmap> cached;
    cached.setMaxCost(64);
    return cached;
}

qint64 cacheKey(int radius, const QColor &color, const QxTheme::QxShadow &shadow)
{
    qint64 key = ::qHash(radius);
    key = ::qHash(int(shadow.blurRadius), key);
    key = ::qHash(shadow.alpha, key);
    key = ::qHash(color.rgba(), key);
    return key;
}
}

QPixmap tile(qreal cornerRadius, const QColor &color, const QxTheme::QxShadow &shadow)
{
    if (!color.isValid() || shadow.alpha <= 0 || shadow.blurRadius <= 0.0) {
        return QPixmap();
    }
    const int radius = int(qBound(0.0, cornerRadius, 64.0));
    const qint64 key = cacheKey(radius, color, shadow);
    if (QPixmap *cached = cache().object(key)) {
        return *cached;
    }
    QPixmap *rendered = new QPixmap(renderTile(radius, color, shadow));
    cache().insert(key, rendered, 1);
    return *rendered;
}

void paint(QPainter *painter, const QRectF &rect, qreal cornerRadius, const QColor &color,
           const QxTheme::QxShadow &shadow)
{
    if (!painter) {
        return;
    }
    const QPixmap source = tile(cornerRadius, color, shadow);
    if (source.isNull()) {
        return;
    }
    const int radius = int(qBound(0.0, cornerRadius, 64.0));
    const int margin = spread(shadow);
    const int corner = radius + margin;
    const int side = coreSide(radius);

    // The shadow sits where the control sits, dropped by the tier's offset.
    const QRectF outer = rect.adjusted(-margin, -margin + shadow.yOffset, margin, margin + shadow.yOffset);
    const qreal x0 = outer.left();
    const qreal y0 = outer.top();
    const qreal x1 = outer.right() - corner;
    const qreal y1 = outer.bottom() - corner;

    painter->save();
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    const auto piece = [&](const QRect &src, qreal x, qreal y, qreal w, qreal h) {
        painter->drawPixmap(QRectF(x, y, w, h), source, QRectF(src));
    };

    if (outer.width() < 2 * corner || outer.height() < 2 * corner) {
        // Too small to slice: one stretch of the whole tile is the honest
        // answer, and a shadow that small reads as a soft edge anyway.
        painter->drawPixmap(outer, source, QRectF(source.rect()));
        painter->restore();
        return;
    }

    piece(QRect(0, 0, corner, corner), x0, y0, corner, corner);
    piece(QRect(corner + side, 0, corner, corner), x1, y0, corner, corner);
    piece(QRect(0, corner + side, corner, corner), x0, y1, corner, corner);
    piece(QRect(corner + side, corner + side, corner, corner), x1, y1, corner, corner);
    piece(QRect(corner, 0, side, corner), x0 + corner, y0, x1 - x0 - corner, corner);
    piece(QRect(corner, corner + side, side, corner), x0 + corner, y1, x1 - x0 - corner, corner);
    piece(QRect(0, corner, corner, side), x0, y0 + corner, corner, y1 - y0 - corner);
    piece(QRect(corner + side, corner, corner, side), x1, y0 + corner, corner, y1 - y0 - corner);
    piece(QRect(corner, corner, side, side), x0 + corner, y0 + corner, x1 - x0 - corner, y1 - y0 - corner);

    painter->restore();
}

}   // namespace QxShadowPainter

QX_CONTROLS_END_NAMESPACE
