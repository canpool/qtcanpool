/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentcard.h"

#include "fluentreveal.h"

#include "qxcontrols/qxshadowpainter.h"

#include <QtGui/QPainter>
#include <QtMath>

QX_FLUENT_BEGIN_NAMESPACE

class FluentCardPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentCard)

    FluentElevation::Level elevation = FluentElevation::Level::Depth4;
    FluentReveal *reveal = Q_NULLPTR;
};

FluentCard::FluentCard(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new FluentCardPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // A card is a surface, not a stop on the way through the keyboard.
    setFocusPolicy(Qt::NoFocus);
    setElevation(d_ptr->elevation);
    // The reveal is what the pointer does to a card, so it is part of the card
    // rather than something applied to one: it is this card's own, parented to
    // it, and it goes away with it.
    d_ptr->reveal = new FluentReveal(this);
}

FluentCard::~FluentCard(){QX_FINI_PRIVATE()}

FluentElevation::Level FluentCard::elevation() const
{
    return d_ptr->elevation;
}

int FluentCard::shadowMargin(FluentElevation::Level level)
{
    const QxTheme::QxShadow shadow = FluentElevation::shadow(level);
    if (shadow.alpha <= 0 || shadow.blurRadius <= 0.0) {
        // A card on the page casts nothing and owes no room.
        return 0;
    }
    // The shadow reaches a blur beyond the surface and sits its offset under
    // it, so the room the card has to keep is both.
    return int(qCeil(shadow.blurRadius)) + 1 + int(qCeil(shadow.yOffset));
}

void FluentCard::setElevation(FluentElevation::Level level)
{
    d_ptr->elevation = level;
    // The margin is not decoration: it is the room the shadow needs, and it is
    // what keeps the card's contents off the shadow they would otherwise sit
    // on top of.
    const int margin = shadowMargin(level);
    setContentsMargins(margin, margin, margin, margin);
    update();
}

qreal FluentCard::cornerRadius() const
{
    return FluentShape::radius(FluentShape::Radius::XLarge);
}

QColor FluentCard::washColor() const
{
    // No flat wash: a card wears the reveal instead, which is the same ink
    // shaped by where the pointer is. An invalid colour is what the base reads
    // as "this control draws its own", so the two never stack.
    return QColor();
}

void FluentCard::paintSurface(QPainter *painter)
{
    const QRectF surface = QRectF(contentsRect());
    const QxTheme::QxShadow shadow = FluentElevation::shadow(d_ptr->elevation);
    if (shadow.alpha > 0 && shadow.blurRadius > 0.0) {
        // The shadow is the ink of the page, thinned by the elevation's own
        // alpha - a card does not carry a shadow colour of its own.
        QxControls::QxShadowPainter::paint(painter, surface, cornerRadius(), textColor(), shadow);
    }
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(palette().color(QPalette::Window));
    const qreal radius = cornerRadius();
    painter->drawRoundedRect(surface, radius, radius);
    // Lit from wherever the pointer is, over the surface and under what the
    // card is holding.
    d_ptr->reveal->paint(painter, surface, radius, textColor());
    painter->restore();
}

QAccessible::Role FluentCard::accessibleRole() const
{
    return QAccessible::Grouping;
}

QX_FLUENT_END_NAMESPACE
