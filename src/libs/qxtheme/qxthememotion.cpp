/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxthememotion.h"

QX_THEME_BEGIN_NAMESPACE

namespace
{
bool reduced = false;
qreal scale = 1.0;
}

bool isMotionReduced()
{
    return reduced;
}

void setMotionReduced(bool on)
{
    reduced = on;
}

qreal motionScale()
{
    return scale;
}

void setMotionScale(qreal s)
{
    scale = s;
}

QxMotion motionFor(int tier)
{
    if (reduced) {
        return QxMotion{0, QEasingCurve::Linear};
    }
    if (tier <= 0) {
        tier = 0;
    } else {
        const int last = int(sizeof(motions) / sizeof(motions[0])) - 1;
        tier = tier < last ? tier : last;
    }
    const QxMotion &base = motions[tier];
    // A scale of nothing is an instant change, the same answer reduce gives:
    // both mean "do not be seen moving".
    return QxMotion{int(base.duration * (scale > 0.0 ? scale : 0.0)), base.curve};
}

QX_THEME_END_NAMESPACE
