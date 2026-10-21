/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluenttokens.h"

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// The corner scale, in the order of the enum. 9999 is a radius no rectangle
// reaches, which is what "fully round" means once it is expressed as one.
constexpr qreal radii[] = {0.0, 2.0, 4.0, 6.0, 8.0, 9999.0};

// The type ramp, in the order of the enum: Fluent's sizes, in points.
constexpr qreal rampSizes[] = {12.0, 14.0, 20.0, 28.0, 40.0};

// Fluent's elevation scale, in the order of the enum: the spread, and the
// darkness that goes with it as an alpha over the shadow colour. The offset is
// half the spread - a surface that lifts further also sits further below it.
constexpr QxTheme::QxShadow shadows[] = {{0.0, 0.0, 0},   {2.0, 1.0, 20},   {4.0, 2.0, 26},  {8.0, 4.0, 31},
                                         {16.0, 8.0, 38}, {28.0, 14.0, 46}, {64.0, 32.0, 61}};

// Which shared motion tier each of Fluent's durations rides on. The ladder is
// three tiers long; the scale is seven steps, so neighbours share a tier -
// which is the honest answer, since the tier is what the theme retimes and the
// millisecond is not.
constexpr int motionTiers[] = {0, 0, 0, 1, 1, 2, 2};

template <typename T, std::size_t N> constexpr std::size_t tableSize(const T (&)[N])
{
    return N;
}
}   // namespace

namespace FluentShape
{

qreal radius(Radius radius)
{
    const int index = int(radius);
    if (index < 0 || index >= int(tableSize(radii))) {
        return 0.0;
    }
    return radii[index];
}

}   // namespace FluentShape

namespace FluentType
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

}   // namespace FluentType

namespace FluentElevation
{

QxTheme::QxShadow shadow(Level level)
{
    const int index = int(level);
    if (index < 0 || index >= int(tableSize(shadows))) {
        return QxTheme::shadowNone;
    }
    return shadows[index];
}

}   // namespace FluentElevation

namespace FluentMotion
{

QxTheme::QxMotion forDuration(Duration duration)
{
    const int index = int(duration);
    if (index < 0 || index >= int(tableSize(motionTiers))) {
        return QxTheme::motionFor(0);
    }
    // motionFor() is the reduce-aware tier lookup: the scale and the switch
    // ride on it, so asking here is the same as asking there.
    return QxTheme::motionFor(motionTiers[index]);
}

}   // namespace FluentMotion

QX_FLUENT_END_NAMESPACE
