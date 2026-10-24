/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxstatelayer.h"

#include <QtGui/QPainter>
#include <QtGui/QPainterPath>

QX_CONTROLS_BEGIN_NAMESPACE

namespace QxStateLayer
{

qreal clampedOpacity(qreal opacity)
{
    if (opacity < 0.0) {
        return 0.0;
    }
    return opacity > 1.0 ? 1.0 : opacity;
}

qreal selectedFillOpacityFor(const QColor &base)
{
    // One fraction is not one step: the wash moves the surface toward the
    // accent by that fraction of the distance between the two, and a dark
    // surface stands nearer a mid-lightness accent than white does. The
    // stronger fraction is what buys back the step the shorter distance costs.
    if (base.isValid() && base.lightnessF() < 0.5) {
        return selectedFillOpacityOnDark;
    }
    return selectedFillOpacity;
}

QColor layered(const QColor &base, const QColor &layer, qreal opacity)
{
    if (!layer.isValid()) {
        return base;
    }
    const QColor under = base.isValid() ? base : QColor(Qt::transparent);
    const qreal o = clampedOpacity(opacity);
    QColor out = under;
    out.setRedF(layer.redF() * o + under.redF() * (1.0 - o));
    out.setGreenF(layer.greenF() * o + under.greenF() * (1.0 - o));
    out.setBlueF(layer.blueF() * o + under.blueF() * (1.0 - o));
    out.setAlphaF(layer.alphaF() * o + under.alphaF() * (1.0 - o));
    return out;
}

void paint(QPainter *painter, const QRectF &rect, const QColor &layer, qreal opacity)
{
    if (!painter || !layer.isValid()) {
        return;
    }
    QColor wash = layer;
    wash.setAlphaF(wash.alphaF() * clampedOpacity(opacity));
    if (wash.alpha() <= 0) {
        return;
    }
    painter->fillRect(rect, wash);
}

}   // namespace QxStateLayer

QX_CONTROLS_END_NAMESPACE
