/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef CUPERTINOTOKENS_H
#define CUPERTINOTOKENS_H

#include "qxcupertino_global.h"

#include "qxtheme/qxthemeelevation.h"
#include "qxtheme/qxthememotion.h"

#include <QtGui/QColor>
#include <QtGui/QFont>

QX_CUPERTINO_BEGIN_NAMESPACE

/*!
 * Cupertino's corner scale, as the radii its controls actually use.
 *
 * The steps are close together on purpose: this family turns a corner by a few
 * pixels and never by a quarter of the control, so the whole scale lives
 * between square and sixteen. A surface that wants to look round is round -
 * the fully round step is expressed as a radius no rectangle can reach, which
 * is the only way to say "a pill" in a value a painter takes.
 *
 * These are not the theme's corner tiers. Those were pinned to the corners the
 * bundled chrome already had and are read by the style sheet; a control drawn
 * in code asks this scale instead.
 */
namespace CupertinoShape
{

enum class Radius {
    None,    /*!< Square: an edge that is the page's. */
    Small,   /*!< 6 - the smallest turn this family draws. */
    Medium,  /*!< 8 - a control. */
    Large,   /*!< 10 - a control with room, a sheet. */
    XLarge,  /*!< 16 - a card, a surface that sits on the page. */
    Circular /*!< Fully round: a pill, a knob. */
};

/*! The radius \a radius asks for, in device independent pixels. */
qreal QX_CUPERTINO_EXPORT radius(Radius radius);

}   // namespace CupertinoShape

/*!
 * Cupertino's type ramp.
 *
 * The ramp is sizes *and* weights, as it is in every family here: \c strong
 * asks for the heavier cut of a step at that step's own size, so a line can be
 * emphasised without being made bigger.
 *
 * The sizes are this family's own reading, not the platform's desktop point
 * values copied across: a control in a dense widget UI wants a body a shade
 * smaller than a full-width reading line, and the steps are chosen so that a
 * caption, a label and a heading are three sizes anyone can tell apart at a
 * glance. No font is carried - a ramp is a size and a weight, and the typeface
 * is the application's answer.
 */
namespace CupertinoType
{

enum class Ramp {
    Caption,  /*!< 12 - the small print under a control. */
    Body,     /*!< 15 - what a control says. */
    Subtitle, /*!< 20 - a heading inside a surface. */
    Title,    /*!< 28 - a heading above one. */
    Display   /*!< 34 - the largest step a widget asks for. */
};

/*! The font for \a ramp: the application's font at that step's size, in the
 *  heavier cut when \a strong is set. */
QFont QX_CUPERTINO_EXPORT font(Ramp ramp, bool strong = false);

}   // namespace CupertinoType

/*!
 * Cupertino's elevation scale: how far above the page a surface sits, spelled
 * as the shadow that lift casts.
 *
 * What makes this scale this family's is the proportion, not the number of
 * levels: the shadows are wide and faint and sit close under the surface, so a
 * raised card reads as lit rather than as cut out. The alternative - a tight,
 * dark shadow - is what a flatter family draws.
 *
 * Each level names the height it stands for in points; the spread, the offset
 * and the darkness are the shadow drawn for it. A level's offset stays under
 * half its spread, which is the proportion above written as a value.
 */
namespace CupertinoElevation
{

enum class Level {
    None,     /*!< On the page: nothing is cast. */
    Raised,   /*!< 1 - the lift a rest state has. */
    Floating, /*!< 4 - a card the pointer is on, a popover. */
    Sheet,    /*!< 10 - a sheet over the page. */
    Modal     /*!< 24 - a modal surface, the one level that dims what is under it. */
};

/*! The shadow \a level asks for. */
QxTheme::QxShadow QX_CUPERTINO_EXPORT shadow(Level level);

}   // namespace CupertinoElevation

/*!
 * Cupertino's duration and easing scale.
 *
 * A movement here is a duration and a curve together, and the curve is the
 * half that is this family's: its moves leave and arrive at the same rate,
 * because a thing that appears to slide across a surface has no reason to
 * brake at one end and not the other. An entrance is the exception and gets a
 * curve that settles.
 *
 * Durations are answered in the shared tiers, so a control that asks here is
 * retimed by the theme's motion scale and silenced by the reduce-motion switch
 * without checking anything itself. The ladder is three tiers long and the
 * scale is four steps, so the two longest share the longest tier - which is
 * the honest answer, since the tier is what gets retimed and the millisecond
 * is not.
 */
namespace CupertinoMotion
{

enum class Duration {
    Quick,   /*!< 100 - a press, a state change in place. */
    Normal,  /*!< 200 - a control moving. */
    Gentle,  /*!< 300 - a change the eye should follow. */
    Entrance /*!< 400 - something arriving over the page. */
};

/*! The curve a movement rides on. */
enum class Easing {
    Standard,  /*!< The symmetric one: what travels within a surface. */
    Emphasized /*!< The one that settles: what arrives over the page. */
};

/*! The motion \a duration asks for, on \a easing's curve, in the shared
 *  vocabulary. */
QxTheme::QxMotion QX_CUPERTINO_EXPORT forDuration(Duration duration, Easing easing = Easing::Standard);

}   // namespace CupertinoMotion

/*!
 * Cupertino's semantic colours: the five the system itself answers with.
 *
 * These are here and not on the palette because they are not the theme's to
 * choose. A switch is green when it is on in any theme this family ships with;
 * making that follow a theme's accent would be a different design language
 * wearing this one's shapes. Surface, text and accent still come from the
 * palette and from QxControl's colour entries - what is fixed here is only
 * what the system says a colour *means*.
 *
 * Each role answers twice, because the system's colours come in a light and a
 * dark cut and the pair is the point: green on white and green on black are two
 * different greens, and a control that picked one for both would be wrong on
 * one of them. \c onDark asks for the second.
 */
namespace CupertinoColor
{

enum class Role {
    Accent, /*!< The system's own blue: a link, a selected row, an active control. */
    Green,  /*!< Yes, on, done. */
    Red,    /*!< No, destructive, failed. */
    Orange, /*!< Careful, and something to finish later. */
    Gray    /*!< Off, disabled-looking, and the ink that is not quite text. */
};

/*! The colour \a role means, in its light cut or its dark one. An unknown role
 *  answers an invalid colour rather than a guess. */
QColor QX_CUPERTINO_EXPORT color(Role role, bool onDark = false);

}   // namespace CupertinoColor

QX_CUPERTINO_END_NAMESPACE

#endif   // CUPERTINOTOKENS_H
