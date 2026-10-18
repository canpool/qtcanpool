/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxapp_global.h"

#include "qxdock/dockstylesheet.h"
#include "qxtheme/qxtheme.h"

#include <QtCore/QString>

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

/*!
 * The whole theme as one application style sheet: the theme's own rules, then
 * the dock's.
 *
 * There is only one application-level sheet, so an application that wants both
 * has to join them, and the order is not cosmetic. At equal specificity the
 * rule declared first wins, so a theme that states something about a widget the
 * dock's fragments also describe keeps the last word - the other order would let
 * the dock's plain background win over the theme's.
 *
 * The dock's focus fragment follows DockManager::FocusHighlighting, the same flag
 * the dock's behaviour follows, so the sheet cannot ask for a frame the dock
 * never draws.
 *
 * A theme with no sheet - an id this build does not register, or an application
 * that registers none at all - yields the dock's fragment alone, whose colours
 * then resolve from the application palette. That is the look the dock had
 * before themes carried colour roles, so such an application loses nothing by
 * calling this.
 */
QX_APP_EXPORT QString applicationStyleSheet(const QxTheme::QxThemeInfo &theme);

QX_APP_END_NAMESPACE
