/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "qxdockstyle.h"

#include "qxdock/dockmanager.h"

QX_APP_BEGIN_NAMESPACE

QX_DOCK_PREPEND_NAMESPACE(DockStyleColors) dockStyleColors(const QxTheme::QxThemeInfo &theme)
{
    QX_DOCK_PREPEND_NAMESPACE(DockStyleColors) colors;
    const auto role = [&theme](const char *name) {
        return theme.colors.value(QLatin1String(name));
    };

    // The dock's surface is the ribbon's *sub* surface: a tab strip sits one
    // step below the page, and a dock tab sits on a panel the same way.
    colors.surface = role("surface-sub");
    colors.surfaceRaised = role("surface");
    colors.divider = role("border");
    colors.text = role("text");
    colors.mutedText = role("text-muted");
    colors.accent = role("accent");
    colors.accentText = role("accent-text");

    // What reads on an accent fill is the ribbon's text-on-fill, and the two
    // washes are that same colour at two opacities - the relationship the dock
    // sheets describe as "a wash of on-accent".
    const QColor onAccent = role("text-on-fill");
    colors.onAccent = onAccent;
    QColor onAccentHover = onAccent;
    onAccentHover.setAlpha(60);
    colors.onAccentHover = onAccentHover;
    QColor onAccentPressed = onAccent;
    onAccentPressed.setAlpha(92);
    colors.onAccentPressed = onAccentPressed;

    colors.subtle = role("state-hover");
    colors.hover = role("state-hover");
    colors.pressed = role("state-pressed");
    colors.tabActiveTop = role("surface-sub");
    colors.tabActiveBottom = role("surface");
    return colors;
}

QString applicationStyleSheet(const QxTheme::QxThemeInfo &theme)
{
    // Read here rather than taken as an argument: the flag is what the dock's
    // behaviour follows, so a sheet rendered from anywhere else would be a
    // second opinion about the same question.
    const bool focus = QX_DOCK_PREPEND_NAMESPACE(DockManager)::testConfigFlag(
        QX_DOCK_PREPEND_NAMESPACE(DockManager)::FocusHighlighting);
    return theme.styleSheet + QX_DOCK_PREPEND_NAMESPACE(dockStyleSheet)(dockStyleColors(theme), focus);
}

QX_APP_END_NAMESPACE
