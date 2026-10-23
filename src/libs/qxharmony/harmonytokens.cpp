/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "harmonytokens.h"

QX_HARMONY_BEGIN_NAMESPACE

namespace
{
// The corner scale, in the order of the enum. 9999 is a radius no rectangle
// reaches, which is what "fully round" means once it is expressed as one.
constexpr qreal radii[] = {0.0, 4.0, 8.0, 16.0, 24.0, 9999.0};

// The type ramp, in the order of the enum, in points.
constexpr qreal rampSizes[] = {12.0, 14.0, 20.0, 28.0, 40.0};

// The elevation scale, in the order of the enum: the spread, the offset and the
// darkness. A surface here stands up rather than being lit, so the spread is
// tighter and the darkness higher than a family that draws a lift as light.
constexpr QxTheme::QxShadow shadows[] = {
    {0.0, 0.0, 0}, {8.0, 4.0, 28}, {16.0, 8.0, 36}, {32.0, 16.0, 44}, {56.0, 28.0, 52}};

// Which shared motion tier each of the family's durations rides on: three steps
// and three tiers, so the two scales agree - which is not something to rely on,
// since a tier is what gets retimed and a step is a name.
constexpr int motionTiers[] = {0, 1, 2};

// The curves, in the order of the enum. Springy overshoots and settles back,
// which is the one movement here that is not a slide.
constexpr QEasingCurve::Type curves[] = {QEasingCurve::OutCubic, QEasingCurve::OutBack};

// The semantic colours, in the order of the enum: the light cut and the dark
// one. Keeping them as values rather than as QColor objects means the library
// holds no static QColor to be constructed and destroyed around main().
struct SemanticPair {
    QRgb light;
    QRgb dark;
};

constexpr SemanticPair semantics[] = {
    {0x007DFF, 0x3D8BFF},   // Accent: the brand's blue.
    {0x00B42A, 0x23C343},   // Green: yes, on, done.
    {0xF53F3F, 0xF76560},   // Red: no, destructive, failed.
    {0xFF7D00, 0xFF9A2E},   // Orange: careful, unfinished.
    {0x86909C, 0xA9AEB8},   // Gray: off, and the ink that is not quite text.
};

template <typename T, std::size_t N> constexpr std::size_t tableSize(const T (&)[N])
{
    return N;
}
}   // namespace

namespace HarmonyShape
{

qreal radius(Radius radius)
{
    const int index = int(radius);
    if (index < 0 || index >= int(tableSize(radii))) {
        return 0.0;
    }
    return radii[index];
}

}   // namespace HarmonyShape

namespace HarmonyType
{

QFont font(Ramp ramp, bool strong)
{
    const int index = int(ramp);
    if (index < 0 || index >= int(tableSize(rampSizes))) {
        return QFont();
    }
    // The family stays whatever the application set: a ramp is a size and a
    // weight, and the typeface is the application's answer, not the ramp's.
    QFont f;
    f.setPointSizeF(rampSizes[index]);
    // Text here leans forward: the base weight is a step above normal, and
    // "strong" is the step above that, at the same size.
    f.setWeight(strong ? QFont::DemiBold : QFont::Medium);
    return f;
}

}   // namespace HarmonyType

namespace HarmonyElevation
{

QxTheme::QxShadow shadow(Level level)
{
    const int index = int(level);
    if (index < 0 || index >= int(tableSize(shadows))) {
        return QxTheme::shadowNone;
    }
    return shadows[index];
}

}   // namespace HarmonyElevation

namespace HarmonyMotion
{

QxTheme::QxMotion forDuration(Duration duration, Easing easing)
{
    const int index = int(duration);
    if (index < 0 || index >= int(tableSize(motionTiers))) {
        return QxTheme::motionFor(0);
    }
    // The duration comes from the ladder, because that is where the theme's
    // scale and the reduce-motion switch are applied; the curve is this
    // family's, because a tier carries a curve of its own.
    QxTheme::QxMotion motion = QxTheme::motionFor(motionTiers[index]);
    // With no duration there is no movement to shape, and the ladder's answer -
    // linear - is the honest one: a curve on a zero-length move is a promise
    // that nothing will happen, differently.
    if (motion.duration > 0) {
        const int curve = int(easing);
        if (curve >= 0 && curve < int(tableSize(curves))) {
            motion.curve = curves[curve];
        }
    }
    return motion;
}

}   // namespace HarmonyMotion

namespace HarmonyColor
{

QColor color(Role role, bool onDark)
{
    const int index = int(role);
    if (index < 0 || index >= int(tableSize(semantics))) {
        return QColor();
    }
    return QColor(onDark ? semantics[index].dark : semantics[index].light);
}

}   // namespace HarmonyColor

QX_HARMONY_END_NAMESPACE
