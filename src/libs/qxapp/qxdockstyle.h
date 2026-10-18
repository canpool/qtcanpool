/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxapp_global.h"

#include "qxdock/dockstylesheet.h"
#include "qxtheme/qxtheme.h"

QX_APP_BEGIN_NAMESPACE

/*!
 * Maps a theme's colour roles onto the dock's colour set.
 *
 * The two sets answer different questions and are deliberately not the same
 * type: a theme's roles describe the ribbon's layering (a tab strip, the page
 * below it, the state fills of a button), while the dock's describe its own
 * (the fill a tab sits on, the step a side tab mark takes). This function is the
 * only place the two are related - adding a theme role that the dock should
 * follow means adding one line here, not touching qxdock.
 *
 * A theme that carries no roles (one registered with a sheet only) yields an
 * all-invalid set, which the renderer and the icon tinting both fill from the
 * palette. That is the same behaviour the dock had before themes carried roles,
 * so such an application loses nothing.
 */
QX_APP_EXPORT QX_DOCK_PREPEND_NAMESPACE(DockStyleColors) dockStyleColors(const QxTheme::QxThemeInfo &theme);

QX_APP_END_NAMESPACE
