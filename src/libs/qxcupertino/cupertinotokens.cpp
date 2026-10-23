/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "cupertinotokens.h"

QX_CUPERTINO_BEGIN_NAMESPACE

namespace
{
// The corner scale, in the order of the enum. 9999 is a radius no rectangle
// reaches, which is what "fully round" means once it is expressed as one.
constexpr qreal radii[] = {0.0, 6.0, 8.0, 10.0, 16.0, 9999.0};

// The type ramp, in the order of the enum, in points.
constexpr qreal rampSizes[] = {12.0, 15.0, 20.0, 28.0, 34.0};

// The elevation scale, in the order of the enum: the spread, the offset and the
// darkness. The offset is well under half the spread - a wide, close shadow is
// what makes this family's lift read as light rather than as a cut-out edge.
constexpr QxTheme::QxShadow shadows[] = {
    {0.0, 0.0, 0}, {6.0, 2.0, 16}, {16.0, 4.0, 22}, {30.0, 8.0, 28}, {60.0, 16.0, 36}};

// Which shared motion tier each of the family's durations rides on. The ladder
// is three tiers long and the scale is four steps, so the last two share the
// longest tier.
constexpr int motionTiers[] = {0, 1, 2, 2};

// The curves, in the order of the enum. Both are symmetric: a movement here
// leaves and arrives at the same rate. Emphasized settles harder at the far
// end, which is what an entrance wants and a state change does not.
constexpr QEasingCurve::Type curves[] = {QEasingCurve::InOutCubic, QEasingCurve::InOutQuint};

// The semantic colours, in the order of the enum: the light cut and the dark
// one. Keeping them as values rather than as QColor objects means the library
// holds no static QColor to be constructed and destroyed around main().
struct SemanticPair {
    QRgb light;
    QRgb dark;
};

constexpr SemanticPair semantics[] = {
    {0x007AFF, 0x0A84FF},   // Accent: the system's blue.
    {0x34C759, 0x30D158},   // Green: yes, on, done.
    {0xFF3B30, 0xFF453A},   // Red: no, destructive, failed.
    {0xFF9500, 0xFF9F0A},   // Orange: careful, unfinished.
    {0x8E8E93, 0x98989D},   // Gray: off, and the ink that is not quite text.
};

template <typename T, std::size_t N> constexpr std::size_t tableSize(const T (&)[N])
{
    return N;
}
}   // namespace

namespace CupertinoShape
{

qreal radius(Radius radius)
{
    const int index = int(radius);
    if (index < 0 || index >= int(tableSize(radii))) {
        return 0.0;
    }
    return radii[index];
}

}   // namespace CupertinoShape

namespace CupertinoType
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
    if (strong) {
        f.setWeight(QFont::DemiBold);
    }
    return f;
}

}   // namespace CupertinoType

namespace CupertinoElevation
{

QxTheme::QxShadow shadow(Level level)
{
    const int index = int(level);
    if (index < 0 || index >= int(tableSize(shadows))) {
        return QxTheme::shadowNone;
    }
    return shadows[index];
}

}   // namespace CupertinoElevation

namespace CupertinoMotion
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

}   // namespace CupertinoMotion

namespace CupertinoColor
{

QColor color(Role role, bool onDark)
{
    const int index = int(role);
    if (index < 0 || index >= int(tableSize(semantics))) {
        return QColor();
    }
    return QColor(onDark ? semantics[index].dark : semantics[index].light);
}

}   // namespace CupertinoColor

QX_CUPERTINO_END_NAMESPACE
