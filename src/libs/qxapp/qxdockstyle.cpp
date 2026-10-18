/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "qxdockstyle.h"

#include "qxdock/dockmanager.h"
#include "qxtheme/qxthememanager.h"

QX_APP_BEGIN_NAMESPACE

QString applicationStyleSheet(QxTheme::QxThemeManager *engine)
{
    // The dock's half is rendered from the palette, not from the theme: the
    // engine put the theme's accent in the palette as QPalette::Highlight, and
    // a dock's own sheet asks the palette for exactly what a control asks for -
    // the colour this application emphasizes with, and the pair that reads on
    // it. Which is why the two halves cannot disagree about the accent any
    // more, and why there is nothing to map.
    //
    // The flag is read here rather than taken as an argument: it is what the
    // dock's behaviour follows, so a sheet rendered from anywhere else would be
    // a second opinion about the same question.
    const bool focus = QX_DOCK_PREPEND_NAMESPACE(DockManager)::testConfigFlag(
        QX_DOCK_PREPEND_NAMESPACE(DockManager)::FocusHighlighting);
    return engine->styleSheet(engine->theme()) + QX_DOCK_PREPEND_NAMESPACE(dockStyleSheet)(focus);
}

QX_APP_END_NAMESPACE
