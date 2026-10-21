/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXTHEMEMOTION_H
#define QXTHEMEMOTION_H

#include "qxtheme_global.h"

#include <QtCore/QEasingCurve>

QX_THEME_BEGIN_NAMESPACE

/*!
 * The motion vocabulary: how long a transition takes and how it moves.
 *
 * A duration and a curve go together the way a colour and the colour that
 * reads on it do - a hover needs a short ease-out, an entrance a longer one
 * with a settle - so the tiers carry both, and a control names a tier instead
 * of spelling milliseconds into its own corner. The tiers are named by what
 * the movement is for, not by how long it lasts: a theme that retimes its
 * motion changes the values a user reads, not the names the code reads.
 */
struct QxMotion
{
    int duration;               /*!< Milliseconds; 0 means jump to the end. */
    QEasingCurve::Type curve;   /*!< The easing the duration rides on. */
};

/*! A state change the user did not ask to watch: hovers, state layers. 120 ms, ease-out. */
inline constexpr QxMotion motionFast{120, QEasingCurve::OutQuad};
/*! A change with distance to cover: expansions, fades, slides. 240 ms, a smoother out. */
inline constexpr QxMotion motionNormal{240, QEasingCurve::OutCubic};
/*! An entrance the user asked for: a flyout opening, a dialog arriving. 360 ms, with a settle. */
inline constexpr QxMotion motionEmphasized{360, QEasingCurve::OutBack};

/*! The tiers in duration order, fast first - a table, not a chain of ifs. */
inline constexpr QxMotion motions[] = {motionFast, motionNormal, motionEmphasized};

/*!
 * True once the application has said the user wants less motion.
 *
 * Widgets has no way to read the system's reduce-motion preference, so the
 * application sets this when it knows - from its own accessibility settings
 * page, or from whatever told it - and QxThemeManager::setReduceMotion()
 * remembers the choice in the configuration file. The switch is process-wide
 * because motion is: a flyout that settles while the dialog over it jumps
 * reads as a bug.
 */
bool QX_THEME_EXPORT isMotionReduced();

/*! Answers \a on to isMotionReduced(); see there for who sets it and why. */
void QX_THEME_EXPORT setMotionReduced(bool on);

/*!
 * The multiplier every duration reads through, 1.0 while nobody says otherwise.
 *
 * This is how a theme retimes the motion of the whole application: the field
 * rides on QxThemeInfo, and QxThemeManager::apply() installs the applied
 * theme's scale here the way it installs the theme's palette - so a theme that
 * wants everything snappier scales it down, one that wants it stately scales
 * it up, and one that says nothing leaves the tiers as they are.
 */
qreal QX_THEME_EXPORT motionScale();
/*! Answers \a scale to motionScale(); 0 or less reads as instant. */
void QX_THEME_EXPORT setMotionScale(qreal scale);

/*!
 * The motion \a tier asks for, saturating at \c motionEmphasized and reading
 * as \c motionFast below zero - a caller that walks a step scale does not have
 * to clamp it first.
 *
 * The answer is what the reduce-motion switch asks it to be: while motion is
 * reduced every tier comes back with a zero duration and a linear curve, so an
 * animator that reads its motion here jumps to the end state without checking
 * the switch itself - compliance is the default, not something every control
 * remembers to do. The scale rides along the same way: the duration that comes
 * back is the tier's, times motionScale().
 */
QxMotion QX_THEME_EXPORT motionFor(int tier);

// The tiers have to keep their order: a caller that walks the table reads the
// order as meaning, the way the shadow tiers' table is read.
static_assert(motionFast.duration < motionNormal.duration
                  && motionNormal.duration < motionEmphasized.duration,
              "motion duration must grow with the tier");

QX_THEME_END_NAMESPACE

#endif   // QXTHEMEMOTION_H
