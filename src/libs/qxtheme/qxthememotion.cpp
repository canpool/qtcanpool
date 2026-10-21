/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxthememotion.h"

QX_THEME_BEGIN_NAMESPACE

namespace
{
bool reduced = false;
}

bool isMotionReduced()
{
    return reduced;
}

void setMotionReduced(bool on)
{
    reduced = on;
}

QxMotion motionFor(int tier)
{
    if (reduced) {
        return QxMotion{0, QEasingCurve::Linear};
    }
    if (tier <= 0) {
        return motionFast;
    }
    const int last = int(sizeof(motions) / sizeof(motions[0])) - 1;
    return motions[tier < last ? tier : last];
}

QX_THEME_END_NAMESPACE
