/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALTOKENS_H
#define MATERIALTOKENS_H

#include "qxmaterial_global.h"

#include "qxtheme/qxthemeelevation.h"
#include "qxtheme/qxthememotion.h"

#include <QtGui/QFont>

QX_MATERIAL_BEGIN_NAMESPACE

/*!
 * Material's corner scale, as the radii its controls actually use.
 *
 * Material 3 arrives with seven of them, and two of the seven - 8 and 12 -
 * differ by four pixels, which is not a difference anyone sees on a control.
 * What is kept is the five that read as different: 0, 4, 8, 16 and 28, with
 * the fully round one expressed as a radius no rectangle reaches.
 */
namespace MaterialShape
{

enum class Radius {
    None,    /*!< Square: a control whose edge is the page's. */
    Small,   /*!< 4 - a chip, the smallest element with a shape. */
    Medium,  /*!< 8 - a button, a text field. */
    Large,   /*!< 16 - a card, a surface that sits on the page. */
    XLarge,  /*!< 28 - a dialog, the one surface that is a place of its own. */
    Circular /*!< Fully round: a pill, a knob, a floating action button. */
};

/*! The radius \a radius asks for, in device independent pixels. */
qreal QX_MATERIAL_EXPORT radius(Radius radius);

}   // namespace MaterialShape

/*!
 * Material's type scale.
 *
 * M3's scale is five roles - display, headline, title, body, label - each in
 * three sizes, and the roles are told apart by what they are *for*, not by how
 * big they are: a title is a title because it names something, not because it
 * is 22. What a control asks for is the role; the size comes with it.
 *
 * \c strong asks for the heavier cut of a role at that role's own size.
 */
namespace MaterialType
{

enum class Scale {
    Label,    /*!< 12 - the small print on a control. */
    Body,     /*!< 14 - what a control says. */
    Title,    /*!< 22 - the name of something. */
    Headline, /*!< 28 - a heading inside a surface. */
    Display   /*!< 36 - the largest step a widget asks for. */
};

/*! The font for \a scale: the application's font at that role's size, in the
 *  heavier cut when \a strong is set. */
QFont QX_MATERIAL_EXPORT font(Scale scale, bool strong = false);

}   // namespace MaterialType

/*!
 * Material's elevation: how far above the page a surface sits, spelled as the
 * shadow that lift casts.
 *
 * Material measures elevation in dp and draws it as a shadow the level names:
 * level 1 is a hairline under a card at rest, level 5 is a dialog lifting off
 * the page. The shadows here are one layer per level, spreading and darkening
 * together, sitting a little below the surface - which is the reading, not
 * the exact two-layer composition Material draws.
 */
namespace MaterialElevation
{

enum class Level {
    None,   /*!< On the page: nothing is cast. A filled card sits here. */
    Level1, /*!< 1 - a surface at rest. */
    Level2, /*!< 3 - a surface the pointer is on. */
    Level3, /*!< 6 - a menu, a surface over the page. */
    Level4, /*!< 8 - a dialog, the one level that dims what is under it. */
    Level5  /*!< 12 - the highest lift Material names, a dragged surface. */
};

/*! The shadow \a level asks for. */
QxTheme::QxShadow QX_MATERIAL_EXPORT shadow(Level level);

}   // namespace MaterialElevation

/*!
 * Material's duration and easing scale.
 *
 * Material spells a movement as a duration *and* a curve: the same 300ms is a
 * different movement with a standard curve than with an emphasized one, which
 * is why the two are asked for together here rather than a duration being
 * mapped onto a curve later.
 *
 * The duration is answered in the shared tiers - so it is retimed by the
 * theme's motion scale and zeroed by the reduce-motion switch - and the curve
 * Material named is what rides on it.
 */
namespace MaterialMotion
{

enum class Duration {
    Short,  /*!< 100 - a state change in place. */
    Medium, /*!< 300 - a control moving. */
    Long    /*!< 500 - an entrance, a change with room to cover. */
};

/*! The curve a movement rides on. Material names three, and they are not
 *  interchangeable: which one a movement wants is part of what it is. */
enum class Easing {
    Standard,  /*<! The one most movements use. */
    Emphasized /*<! The one that settles: entrances, and things that travel. */
};

/*! The motion \a duration asks for, on \a easing's curve, in the shared
 *  vocabulary. */
QxTheme::QxMotion QX_MATERIAL_EXPORT forDuration(Duration duration, Easing easing = Easing::Standard);

}   // namespace MaterialMotion

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALTOKENS_H
