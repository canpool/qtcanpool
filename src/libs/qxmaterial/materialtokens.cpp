/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialtokens.h"

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// The corner scale, in the order of the enum. 9999 is a radius no rectangle
// reaches, which is what "fully round" means once it is expressed as one.
constexpr qreal radii[] = {0.0, 4.0, 8.0, 16.0, 28.0, 9999.0};

// The type scale, in the order of the enum: the size each role carries, in
// points.
constexpr qreal scaleSizes[] = {12.0, 14.0, 22.0, 28.0, 36.0};

// Material's elevation, in the order of the enum: the spread, and the darkness
// that goes with it as an alpha over the shadow colour. A surface that lifts
// further sits further below its own shadow, so the two travel together.
constexpr QxTheme::QxShadow shadows[] = {{0.0, 0.0, 0},  {2.0, 1.0, 22},  {4.0, 2.0, 28},
                                         {8.0, 4.0, 34}, {16.0, 8.0, 41}, {24.0, 12.0, 48}};

// Which shared motion tier each of Material's durations rides on: the ladder
// is three tiers long and the scale is three steps, so the two agree - which
// is not the case in every family, and is not something to rely on.
constexpr int motionTiers[] = {0, 1, 2};

// Material's curves, in the order of the enum. Emphasized is the one that
// settles: it leaves faster than the standard one and arrives slower, which
// is what an entrance wants and a state change does not.
constexpr QEasingCurve::Type curves[] = {QEasingCurve::OutCubic, QEasingCurve::OutQuint};

template <typename T, std::size_t N> constexpr std::size_t tableSize(const T (&)[N])
{
    return N;
}
}   // namespace

namespace MaterialShape
{

qreal radius(Radius radius)
{
    const int index = int(radius);
    if (index < 0 || index >= int(tableSize(radii))) {
        return 0.0;
    }
    return radii[index];
}

}   // namespace MaterialShape

namespace MaterialType
{

QFont font(Scale scale, bool strong)
{
    const int index = int(scale);
    if (index < 0 || index >= int(tableSize(scaleSizes))) {
        return QFont();
    }
    // The family stays whatever the application set: a scale is a size and a
    // weight, and the typeface is the application's answer, not the scale's.
    QFont f;
    f.setPointSizeF(scaleSizes[index]);
    if (strong) {
        f.setWeight(QFont::DemiBold);
    }
    return f;
}

}   // namespace MaterialType

namespace MaterialElevation
{

QxTheme::QxShadow shadow(Level level)
{
    const int index = int(level);
    if (index < 0 || index >= int(tableSize(shadows))) {
        return QxTheme::shadowNone;
    }
    return shadows[index];
}

}   // namespace MaterialElevation

namespace MaterialMotion
{

QxTheme::QxMotion forDuration(Duration duration, Easing easing)
{
    const int index = int(duration);
    if (index < 0 || index >= int(tableSize(motionTiers))) {
        return QxTheme::motionFor(0);
    }
    // The duration is the tier's, because motionFor() is where the theme's
    // scale and the reduce-motion switch are applied; the curve is Material's,
    // because a tier carries a curve of its own and this one is what Material
    // asked for instead.
    QxTheme::QxMotion motion = QxTheme::motionFor(motionTiers[index]);
    // With no duration there is no movement to shape, and the ladder's own
    // answer - linear - is the honest one: a curve on a zero-length move is a
    // promise that nothing will happen, differently.
    if (motion.duration > 0) {
        const int curve = int(easing);
        if (curve >= 0 && curve < int(tableSize(curves))) {
            motion.curve = curves[curve];
        }
    }
    return motion;
}

}   // namespace MaterialMotion

QX_MATERIAL_END_NAMESPACE
