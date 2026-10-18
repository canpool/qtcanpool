/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxapp_global.h"

#include "qxdock/dockstylesheet.h"
#include "qxtheme/qxtheme.h"

#include <QtCore/QString>

namespace QxTheme
{
class QxThemeManager;
}

QX_APP_BEGIN_NAMESPACE

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
 * The dock's half is rendered from the application palette, which the engine
 * filled from the theme in force: not from the sheet, which cannot be asked, and
 * not from a table of roles, which would be a second answer to a question the
 * palette already answers. A theme that registered no accent leaves the palette
 * on the engine's own highlight, and the dock follows that instead - so an
 * application with no registered theme loses nothing by calling this.
 *
 * The dock's focus fragment follows DockManager::FocusHighlighting, the same flag
 * the dock's behaviour follows, so the sheet cannot ask for a frame the dock
 * never draws.
 */
QX_APP_EXPORT QString applicationStyleSheet(QxTheme::QxThemeManager *engine);

QX_APP_END_NAMESPACE
