/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock_global.h"
#include "dockstylesheet.h"

#include <QIcon>
#include <QPalette>
#include <QWidget>

QX_DOCK_BEGIN_NAMESPACE

class DockIconProviderPrivate;

class QX_DOCK_EXPORT DockIconProvider
{
public:
    DockIconProvider();
    ~DockIconProvider();

    QIcon customIcon(Qx::DockIcon id) const;
    void registerCustomIcon(Qx::DockIcon id, const QIcon &icon);
private:
    QX_DECLARE_PRIVATE(DockIconProvider)
};

/*!
 * Recolours the dock's built-in SVG icons to match \a palette and registers them
 * with the icon provider, so every dock button follows the application theme
 * instead of the hard-coded black the SVGs ship with. The docking library no
 * longer imposes a look (it only provides the helper); an application opts in by
 * calling this, the same way it calls dockStyleSheet(). If an application never
 * calls it, the buttons fall back to the platform style icons, which are already
 * theme-aware.
 *
 * Pass \a root (the dock window) to also re-apply the freshly registered icons to
 * buttons that already exist; call applyDockIcons() again with \a root on a
 * palette change so already-created buttons pick up the new colours.
 */
QX_DOCK_EXPORT void applyDockIcons(const QPalette &palette, QWidget *root = nullptr);

/*!
 * The same re-tint, with the colours the dock sheet is rendered with.
 *
 * This is the entry point that keeps the glyphs and the sheet in step: an
 * application that maps a theme onto a DockStyleColors hands the same set to
 * both, so the glyph on an accent fill is the sheet's on-accent colour rather
 * than whatever the palette happens to call HighlightedText. A field left
 * invalid falls back the same way it does in the sheet.
 */
QX_DOCK_EXPORT void applyDockIcons(const DockStyleColors &colors, QWidget *root = nullptr);

/*!
 * Re-applies the provider's icons to any dock button beneath \a root. Call it
 * after applyDockIcons() has registered new icons (for instance on a palette
 * change) so already-created buttons pick them up.
 */
QX_DOCK_EXPORT void refreshDockIcons(QWidget *root);

QX_DOCK_END_NAMESPACE
