/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef HARMONYTOKENS_H
#define HARMONYTOKENS_H

#include "qxharmony_global.h"

#include "qxtheme/qxthemeelevation.h"
#include "qxtheme/qxthememotion.h"

#include <QtGui/QColor>
#include <QtGui/QFont>

QX_HARMONY_BEGIN_NAMESPACE

/*!
 * Harmony's corner scale, as the radii its controls actually use.
 *
 * The corners are large enough to be the shape rather than a softened edge: a
 * control is eight, a card sixteen, and a layer that floats over the page
 * twenty-four, because a surface here is a sheet laid on the page and a sheet
 * has a corner you can see.
 *
 * The steps are not the theme's corner tiers. Those were pinned to the corners
 * the bundled chrome had; a control drawn in code asks this scale instead.
 */
namespace HarmonyShape
{

enum class Radius {
    None,    /*!< Square: an edge that is the page's. */
    Small,   /*!< 4 - the smallest turn this family draws. */
    Medium,  /*!< 8 - a control. */
    Large,   /*!< 16 - a card. */
    XLarge,  /*!< 24 - a sheet, a layer over the page. */
    Circular /*!< Fully round: a pill, a knob. */
};

/*! The radius \a radius asks for, in device independent pixels. */
qreal QX_HARMONY_EXPORT radius(Radius radius);

}   // namespace HarmonyShape

/*!
 * Harmony's type ramp.
 *
 * What separates this ramp from a scale that only carries sizes is weight, and
 * it is carried twice: every step is set at a medium weight, because text here
 * leans forward rather than sitting back, and \c strong asks for the heavier cut
 * at the same size so a line can be emphasised without being made bigger.
 *
 * No font is carried. A ramp is a size and a weight, and the typeface is the
 * application's answer - the family's own typeface is a file, and shipping one
 * would put a licence and a dozen megabytes into every application that draws a
 * switch.
 */
namespace HarmonyType
{

enum class Ramp {
    Caption,  /*!< 12 - the small print under a control. */
    Body,     /*!< 14 - what a control says. */
    Subtitle, /*!< 20 - a heading inside a surface. */
    Title,    /*!< 28 - a heading above one. */
    Display   /*!< 40 - the largest step a widget asks for. */
};

/*! The font for \a ramp: the application's font at that step's size, at the
 *  family's weight, or the heavier cut of it when \a strong is set. */
QFont QX_HARMONY_EXPORT font(Ramp ramp, bool strong = false);

}   // namespace HarmonyType

/*!
 * Harmony's elevation scale: how far above the page a surface sits, spelled as
 * the shadow that lift casts.
 *
 * A surface here stands up rather than being lit: the spread is tighter and the
 * darkness higher than a family that treats a lift as light, because a card is
 * a sheet with an edge under it and the edge is the point. That is the opposite
 * trade from the wide, faint, close-shadow reading, and it is a choice rather
 * than an approximation - the same lift, drawn two ways.
 *
 * Each level names the height it stands for in points; the spread, the offset
 * and the darkness are the shadow drawn for it. The offset grows with the
 * spread, so a higher level sits further below its own edge.
 */
namespace HarmonyElevation
{

enum class Level {
    None,   /*!< On the page: nothing is cast. */
    Raised, /*!< 4 - a surface at rest. */
    Float,  /*!< 8 - a surface the pointer is on. */
    Sheet,  /*!< 16 - a sheet over the page. */
    Modal   /*!< 24 - a modal layer, the one level that dims what is under it. */
};

/*! The shadow \a level asks for. */
QxTheme::QxShadow QX_HARMONY_EXPORT shadow(Level level);

}   // namespace HarmonyElevation

/*!
 * Harmony's duration and easing scale.
 *
 * The durations are answered in the shared tiers, so a control that asks here is
 * retimed by the theme's motion scale and silenced by the reduce-motion switch
 * without checking anything itself.
 *
 * The curve is where this scale says something the others do not: \c Springy
 * arrives with a little more than it needs and settles back. It is the one
 * movement here that is not a slide, and it belongs to the things that land -
 * a knob arriving at its end, a sheet dropping into place - rather than to
 * anything that merely changes colour.
 */
namespace HarmonyMotion
{

enum class Duration {
    Quick,   /*!< 150 - a state change in place. */
    Normal,  /*!< 250 - a control moving. */
    Entrance /*!< 350 - something arriving over the page. */
};

/*! The curve a movement rides on. */
enum class Easing {
    Standard, /*!< The one most movements use. */
    Springy   /*!< The one that overshoots a little and settles. */
};

/*! The motion \a duration asks for, on \a easing's curve, in the shared
 *  vocabulary. */
QxTheme::QxMotion QX_HARMONY_EXPORT forDuration(Duration duration, Easing easing = Easing::Standard);

}   // namespace HarmonyMotion

/*!
 * Harmony's semantic colours: what the system itself says a colour means.
 *
 * They live here and not on the palette because they are not the theme's to
 * choose. "On" is the brand's colour in every theme this family ships with; let
 * it follow the theme's accent and it is a different design language wearing
 * these shapes. Surface, text and accent still come from the palette and from
 * QxControl's colour entries - what is fixed here is only meaning.
 *
 * Each role answers twice, because the system's colours come in a light and a
 * dark cut and the pair is the point: green on white and green on black are two
 * different greens, and a control that picked one for both would be wrong on one
 * of them. \c onDark asks for the second.
 */
namespace HarmonyColor
{

enum class Role {
    Accent, /*!< The brand's blue: a link, a selected destination, an active control. */
    Green,  /*!< Yes, on, done. */
    Red,    /*!< No, destructive, failed. */
    Orange, /*!< Careful, and something to finish later. */
    Gray    /*!< Off, disabled-looking, and the ink that is not quite text. */
};

/*! The colour \a role means, in its light cut or its dark one. An unknown role
 *  answers an invalid colour rather than a guess. */
QColor QX_HARMONY_EXPORT color(Role role, bool onDark = false);

}   // namespace HarmonyColor

QX_HARMONY_END_NAMESPACE

#endif   // HARMONYTOKENS_H
