/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentcard.h"

#include "qxcontrols/qxshadowpainter.h"

#include <QtGui/QPainter>
#include <QtMath>

QX_FLUENT_BEGIN_NAMESPACE

class FluentCardPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentCard)

    FluentElevation::Level elevation = FluentElevation::Level::Depth4;
};

FluentCard::FluentCard(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new FluentCardPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // A card is a surface, not a stop on the way through the keyboard.
    setFocusPolicy(Qt::NoFocus);
    setElevation(d_ptr->elevation);
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
    painter->restore();
}

QAccessible::Role FluentCard::accessibleRole() const
{
    return QAccessible::Grouping;
}

QX_FLUENT_END_NAMESPACE
