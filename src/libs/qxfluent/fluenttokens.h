/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTTOKENS_H
#define FLUENTTOKENS_H

#include "qxfluent_global.h"

#include "qxtheme/qxthemeelevation.h"
#include "qxtheme/qxthememotion.h"

#include <QtGui/QFont>

QX_FLUENT_BEGIN_NAMESPACE

/*!
 * Fluent's corner scale, as the radii its controls actually use.
 *
 * A design language arrives with a scale of its own, and the sheet's corner
 * tiers - which the chrome reads - are not it: those five were pinned to the
 * corners the bundled chrome already had. A control drawn in code asks this
 * scale instead, which is why the two live in different places and why the
 * values differ at all.
 *
 * The steps are Fluent's: 0, 2, 4, 6, 8, and the fully round one expressed as
 * a radius no rectangle can reach.
 */
namespace FluentShape
{

enum class Radius {
    None,    /*!< Square: a control whose edge is the page's. */
    Small,   /*!< 2 - the smallest turn Fluent draws. */
    Medium,  /*!< 4 - a control. */
    Large,   /*!< 6 - a control with room. */
    XLarge,  /*!< 8 - a card, a surface that sits on the page. */
    Circular /*!< Fully round: a pill, a knob. */
};

/*! The radius \a radius asks for, in device independent pixels. */
qreal QX_FLUENT_EXPORT radius(Radius radius);

}   // namespace FluentShape

/*!
 * Fluent's type ramp.
 *
 * The ramp is sizes *and* weights: Fluent's subtitle and title are the same
 * size as their body-strong sibling would be and are told apart by weight, so
 * a scale that only carried sizes would be lying about it. \c strong asks for
 * the heavier cut of a step at that step's own size.
 */
namespace FluentType
{

enum class Ramp {
    Caption,  /*!< 12 - the small print under a control. */
    Body,     /*!< 14 - what a control says. */
    Subtitle, /*!< 20 - a heading inside a surface. */
    Title,    /*!< 28 - a heading above one. */
    Display   /*!< 40 - the largest step a widget asks for. */
};

/*! The font for \a ramp: the application's font at that step's size, in the
 *  heavier cut when \a strong is set. */
QFont QX_FLUENT_EXPORT font(Ramp ramp, bool strong = false);

}   // namespace FluentType

/*!
 * Fluent's elevation scale: how far above the page a surface sits, spelled as
 * the shadow that lift casts.
 *
 * Fluent names its shadows by how far they spread, from a hairline to a
 * dialog's drop, and it draws each one as two layers - an ambient and a key
 * shadow - which is one layer more than a painter here carries. What is kept
 * is the reading: one shadow per level, spreading and darkening together,
 * sitting half its spread below the surface. A family that wanted the two
 * layers back would draw this shadow twice.
 */
namespace FluentElevation
{

enum class Level {
    None,    /*!< On the page: nothing is cast. */
    Depth2,  /*!< 2 - the lift a hover promises. */
    Depth4,  /*!< 4 - a resting card. */
    Depth8,  /*!< 8 - a card the pointer is on. */
    Depth16, /*!< 16 - a menu. */
    Depth28, /*!< 28 - a flyout. */
    Depth64  /*!< 64 - a dialog, the one level that dims what is under it. */
};

/*! The shadow \a level asks for. */
QxTheme::QxShadow QX_FLUENT_EXPORT shadow(Level level);

}   // namespace FluentElevation

/*!
 * Fluent's duration scale.
 *
 * Fluent spells its durations in milliseconds and its curves as beziers; the
 * shared vocabulary answers in tiers. The mapping is the whole content of this
 * scale, and what it buys is worth more than the exact numbers: a duration
 * read through the tier ladder is retimed by the theme's motion scale and
 * zeroed by the reduce-motion switch, so a control that asks here is compliant
 * without remembering to check anything.
 */
namespace FluentMotion
{

enum class Duration {
    UltraFast, /*!< 50 - a state change in place. */
    Faster,    /*!< 100 */
    Fast,      /*!< 150 - a press, a hover. */
    Normal,    /*!< 200 - a control moving. */
    Gentle,    /*!< 300 */
    Slow,      /*!< 400 - an entrance. */
    Slower     /*!< 600 - the longest move Fluent draws. */
};

/*! The motion \a duration asks for, in the shared vocabulary. */
QxTheme::QxMotion QX_FLUENT_EXPORT forDuration(Duration duration);

}   // namespace FluentMotion

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTTOKENS_H
