/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "harmonycard.h"

#include "harmonytokens.h"

#include "qxcontrols/qxshadowpainter.h"

#include <QtGui/QPainter>

QX_HARMONY_BEGIN_NAMESPACE

namespace
{
// How much of the widget is the card's own face and how much is the room its
// shadow needs: the shadow spreads beyond the face, so the face is inset and the
// shadow is drawn in the room around it rather than being cut off at the edge.
constexpr qreal kShadowRoom = 8.0;

// A disabled card keeps its drawing and loses its ink, as in the rest of the
// family.
constexpr qreal kDisabledOpacity = 0.4;
}   // namespace

class HarmonyCardPrivate
{
public:
    QX_DECLARE_PUBLIC(HarmonyCard)

    HarmonyElevation::Level level = HarmonyElevation::Level::Raised;
};

HarmonyCard::HarmonyCard(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new HarmonyCardPrivate){QX_SET_PRIVATE(d_ptr)}

    HarmonyCard::~HarmonyCard(){QX_FINI_PRIVATE()}

    HarmonyElevation::Level HarmonyCard::elevation() const
{
    return d_ptr->level;
}

qreal HarmonyCard::shadowRoom()
{
    return kShadowRoom;
}

void HarmonyCard::setElevation(HarmonyElevation::Level level)
{
    if (d_ptr->level == level) {
        return;
    }
    d_ptr->level = level;
    update();
}

QSize HarmonyCard::sizeHint() const
{
    return QSize(200, 120);
}

QSize HarmonyCard::minimumSizeHint() const
{
    return QSize(int(kShadowRoom * 4.0), int(kShadowRoom * 3.0));
}

qreal HarmonyCard::cornerRadius() const
{
    return HarmonyShape::radius(HarmonyShape::Radius::Large);
}

void HarmonyCard::paintSurface(QPainter *painter)
{
    const QRectF face = QRectF(rect()).adjusted(kShadowRoom, kShadowRoom, -kShadowRoom, -kShadowRoom);
    const qreal radius = cornerRadius();

    painter->save();
    painter->setPen(Qt::NoPen);
    if (!isEnabled()) {
        painter->setOpacity(kDisabledOpacity);
    }

    // The lift, drawn under the face: the family's reading of a surface standing
    // on the page rather than lying in it.
    QxControls::QxShadowPainter::paint(painter, face, radius, textColor(), HarmonyElevation::shadow(d_ptr->level));

    // The face is the toolkit's surface colour rather than the page's, so a card
    // is a different material from what it lies on even where the shadow is thin.
    painter->setBrush(palette().color(QPalette::Base));
    painter->drawRoundedRect(face, radius, radius);

    painter->restore();
}

QAccessible::Role HarmonyCard::accessibleRole() const
{
    return QAccessible::Client;
}

QX_HARMONY_END_NAMESPACE
