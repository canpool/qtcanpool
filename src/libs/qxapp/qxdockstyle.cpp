/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "qxdockstyle.h"

#include "qxdock/dockstylesheet.h"
#include "qxtheme/qxthememanager.h"

QX_APP_BEGIN_NAMESPACE

QString applicationStyleSheet(QxTheme::QxThemeManager *engine)
{
    // The theme's sheet already carries the dock: those rules are one part of a
    // theme, sitting next to the ribbon's in the same file, and the ten bundled
    // themes are generated with them. What is left is only the piece a theme
    // cannot have - the rules that belong to one desktop, which the library
    // knows from the way it was built and the generator, running anywhere, does
    // not. On Windows and macOS that piece is empty and this is the theme's
    // sheet unchanged.
    //
    // Appending the dock's whole sheet instead would draw the same rules twice,
    // and the second copy would win on order while being filled in from the
    // palette rather than from the theme - the fall-back speaking over the
    // source. A theme that says nothing about the dock draws the dock the way a
    // theme that says nothing about the ribbon draws the ribbon; an application
    // that wants the dock's look without a theme installs
    // dockStyleSheet() itself.
    return engine->styleSheet(engine->theme()) + QX_DOCK_PREPEND_NAMESPACE(dockPlatformStyleSheet)();
}

QX_APP_END_NAMESPACE
