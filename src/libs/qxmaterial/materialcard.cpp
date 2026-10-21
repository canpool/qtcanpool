/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialcard.h"

#include "materialtokens.h"

#include "qxcontrols/qxshadowpainter.h"

#include <QtGui/QPainter>
#include <QtMath>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
constexpr qreal kOutlineWidth = 1.0;
constexpr qreal kOutlineOpacity = 0.30;
}   // namespace

class MaterialCardPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialCard)

    MaterialCard::Variant variant = MaterialCard::Variant::Elevated;

    /*! The lift the card is given: only the elevated one has one, and it lifts
     *  one more while the pointer is on it. */
    MaterialElevation::Level level() const
    {
        if (variant != MaterialCard::Variant::Elevated) {
            return MaterialElevation::Level::None;
        }
        return q_ptr->isActive() ? MaterialElevation::Level::Level2 : MaterialElevation::Level::Level1;
    }
    /*! The colour of the surface, out of the palette the theme is running. */
    QColor fill() const
    {
        // Two of the three surfaces are the page's own colour and are told
        // apart by what is round them; the filled one is a step away, which is
        // the palette role that means one step away.
        const QPalette palette = q_ptr->palette();
        return variant == MaterialCard::Variant::Filled ? palette.color(QPalette::AlternateBase)
                                                        : palette.color(QPalette::Window);
    }
};

MaterialCard::MaterialCard(Variant variant, QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new MaterialCardPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // A card is a surface, not a stop on the way through the keyboard.
    setFocusPolicy(Qt::NoFocus);
    setVariant(variant);
}

MaterialCard::~MaterialCard(){QX_FINI_PRIVATE()}

MaterialCard::Variant MaterialCard::variant() const
{
    return d_ptr->variant;
}

int MaterialCard::shadowMargin(Variant variant)
{
    if (variant != Variant::Elevated) {
        // A card that casts nothing owes no room.
        return 0;
    }
    // The room is for the lift the card reaches while the pointer is on it, not
    // for the one it rests at: a shadow clipped because the card went up is a
    // card with a seam round it.
    const QxTheme::QxShadow shadow = MaterialElevation::shadow(MaterialElevation::Level::Level2);
    return int(qCeil(shadow.blurRadius)) + 1 + int(qCeil(shadow.yOffset));
}

void MaterialCard::setVariant(Variant variant)
{
    d_ptr->variant = variant;
    const int margin = shadowMargin(variant);
    setContentsMargins(margin, margin, margin, margin);
    update();
}

qreal MaterialCard::cornerRadius() const
{
    // A card is the surface with the room to turn: sixteen is Material's own
    // reading of a corner on something this size.
    return MaterialShape::radius(MaterialShape::Radius::Large);
}

QColor MaterialCard::washColor() const
{
    // A card takes no wash: what the pointer does to it is lift it, which it
    // draws itself, and a tint on top of that would be two answers to one
    // pointer.
    return QColor();
}

bool MaterialCard::hitTest(const QPointF &pos) const
{
    Q_UNUSED(pos)
    // A card is a surface, not something the pointer presses: what is inside it
    // is what gets pressed, and a card that swallowed the press would keep its
    // contents from answering it.
    return false;
}

void MaterialCard::paintSurface(QPainter *painter)
{
    const QRectF surface = QRectF(contentsRect());
    const qreal radius = cornerRadius();
    const QxTheme::QxShadow shadow = MaterialElevation::shadow(d_ptr->level());

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    if (shadow.alpha > 0 && shadow.blurRadius > 0.0) {
        // The shadow is the ink of the page, thinned by the elevation's own
        // alpha - a card does not carry a shadow colour of its own.
        QxControls::QxShadowPainter::paint(painter, surface, radius, textColor(), shadow);
    }
    painter->setPen(Qt::NoPen);
    painter->setBrush(d_ptr->fill());
    painter->drawRoundedRect(surface, radius, radius);

    if (d_ptr->variant == Variant::Outlined) {
        // The line is the whole of what makes this card a surface: without it
        // there is nothing to find it by.
        QColor outline = textColor();
        outline.setAlphaF(kOutlineOpacity);
        painter->setPen(QPen(outline, kOutlineWidth));
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(surface.adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
    }
    painter->restore();
}

QAccessible::Role MaterialCard::accessibleRole() const
{
    return QAccessible::Grouping;
}

QX_MATERIAL_END_NAMESPACE
