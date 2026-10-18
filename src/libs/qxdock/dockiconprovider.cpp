/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockiconprovider.h"
#include "dockmanager.h"
#include "docktab.h"
#include "dockutils.h"

#include <QAbstractButton>
#include <QFile>
#include <QGuiApplication>
#include <QHash>
#include <QIcon>
#include <QPaintDevice>
#include <QPainter>
#include <QPixmap>
#include <QSvgRenderer>
#include <QVariant>
#include <QVector>

QX_DOCK_BEGIN_NAMESPACE

class DockIconProviderPrivate
{
public:
    QX_DECLARE_PUBLIC(DockIconProvider)
public:
    DockIconProviderPrivate();
public:
    QVector<QIcon> m_userIcons{Qx::NDockIconSizes, QIcon()};
};

DockIconProviderPrivate::DockIconProviderPrivate()
{
}

DockIconProvider::DockIconProvider()
{
    QX_INIT_PRIVATE(DockIconProvider);
}

DockIconProvider::~DockIconProvider()
{
    QX_FINI_PRIVATE();
}

QIcon DockIconProvider::customIcon(Qx::DockIcon id) const
{
    Q_D(const DockIconProvider);
    Q_ASSERT(id < d->m_userIcons.size());
    return d->m_userIcons[id];
}

void DockIconProvider::registerCustomIcon(Qx::DockIcon id, const QIcon &icon)
{
    Q_D(DockIconProvider);
    Q_ASSERT(id < d->m_userIcons.size());
    d->m_userIcons[id] = icon;
}

namespace
{
struct IconSpec {
    Qx::DockIcon id;
    const char *file;
    QIcon::Mode mode;
    QIcon::State state;
};

// The dock's built-in icons are single-colour SVGs. Each file ships with one
// hard-coded colour: black on the normal and the focused state, a grey on the
// disabled one - the focused files are white copies of the normal shape, and
// tintedIcon() maps every painted colour onto one target, because all of the
// states sit on a single known surface. Tinting swaps the shipped colour for
// the palette's, so the icon follows the theme instead of staying black on a
// dark background.
const IconSpec kIconSpecs[] = {
    {Qx::DockPanelCloseIcon, "close-button.svg", QIcon::Normal, QIcon::Off},
    {Qx::DockPanelCloseIcon, "close-button-disabled.svg", QIcon::Disabled, QIcon::Off},
    {Qx::DockPanelCloseIcon, "close-button-focused.svg", QIcon::Active, QIcon::Off},
    {Qx::DockTabCloseIcon, "close-button.svg", QIcon::Normal, QIcon::Off},
    {Qx::DockTabCloseIcon, "close-button-disabled.svg", QIcon::Disabled, QIcon::Off},
    {Qx::DockTabCloseIcon, "close-button-focused.svg", QIcon::Active, QIcon::Off},
    {Qx::DockPanelMenuIcon, "tabs-menu-button.svg", QIcon::Normal, QIcon::Off},
    {Qx::DockPanelUndockIcon, "detach-button.svg", QIcon::Normal, QIcon::Off},
    {Qx::DockPanelUndockIcon, "detach-button-disabled.svg", QIcon::Disabled, QIcon::Off},
    {Qx::DockPanelMinimizeIcon, "minimize-button.svg", QIcon::Normal, QIcon::Off},
    {Qx::DockPanelMinimizeIcon, "minimize-button-focused.svg", QIcon::Active, QIcon::Off},
    {Qx::DockAutoHideIcon, "vs-pin-button.svg", QIcon::Normal, QIcon::Off},
    {Qx::DockAutoHideIcon, "vs-pin-button-disabled.svg", QIcon::Disabled, QIcon::Off},
    {Qx::DockAutoHideIcon, "vs-pin-button-pinned.svg", QIcon::Normal, QIcon::On},
    {Qx::DockAutoHideIcon, "vs-pin-button-pinned-focused.svg", QIcon::Active, QIcon::On},
};

// An SVG colour value for the replacement pass: opaque colours go out as hex,
// translucent ones as rgba(), which is the form the SVG renderer accepts.
QString svgColor(const QColor &color)
{
    if (color.alpha() == 255)
        return color.name();
    return QStringLiteral("rgba(%1, %2, %3, %4)")
        .arg(color.red())
        .arg(color.green())
        .arg(color.blue())
        .arg(color.alphaF(), 0, 'f', 2);
}

QIcon tintedIcon(const QString &fileName, const QColor &glyph, const QColor &glyphDisabled)
{
    QFile file(QStringLiteral(":/qxdock/res/images/") + fileName);
    if (!file.open(QIODevice::ReadOnly))
        return QIcon();
    QString text = QString::fromUtf8(file.readAll());

    // Every painted colour becomes the one target: the black files are the
    // resting shape, the white files the same shape on a hover fill, and both
    // sit on the surface the caller says the icon lives on. Replacement order
    // matters: rewrite the six-digit white before the three-digit one,
    // otherwise "#ffffff" would be partially clobbered by the "#fff" rule. The
    // colour is the only thing that changes; the shape is left untouched, so
    // rendering is identical to the original shipped SVG.
    text.replace(QStringLiteral("#ffffff"), svgColor(glyph));
    text.replace(QStringLiteral("#fff"), svgColor(glyph));
    text.replace(QStringLiteral("#a0a0a0"), svgColor(glyphDisabled));
    text.replace(QStringLiteral("#000000"), svgColor(glyph));

    // Render through the SVG engine at several sizes so the icon stays crisp on
    // hi-DPI screens; a single intrinsic-size raster would blur when scaled.
    QSvgRenderer renderer(text.toUtf8());
    if (!renderer.isValid())
        return QIcon();
    const QSize sizes[] = {QSize(16, 16), QSize(24, 24), QSize(32, 32), QSize(48, 48)};
    QIcon icon;
    for (const QSize &size : sizes) {
        QPixmap pixmap(size);
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        renderer.render(&painter);
        icon.addPixmap(pixmap);
    }
    return icon;
}

// The regular tint: an icon over the surface takes the sheet's text colour,
// whatever mode it is in - hover and press paint the sheet's hover/pressed
// tokens, which belong to the surface's family. The disabled state takes the
// muted text colour, which the theme dims from the same scheme.
QIcon tintedIconForWindow(const QString &fileName, const DockStyleColors &colors)
{
    return tintedIcon(fileName, colors.text, colors.mutedText);
}

// The icon set that reads on the accent fill. The focused tab's close button
// and the whole auto hide title bar sit on that fill, where the text tint
// disappears into the accent. Every state takes on-accent; the disabled state
// steps back to a translucent version of it, the only step available on a
// saturated fill.
QIcon highlightSurfaceIconImpl(const DockStyleColors &colors, Qx::DockIcon id)
{
    QColor glyphDisabled = colors.onAccent;
    glyphDisabled.setAlpha(110);
    QIcon icon;
    for (const IconSpec &spec : kIconSpecs) {
        if (spec.id != id)
            continue;
        QIcon svg = tintedIcon(QLatin1String(spec.file), colors.onAccent, glyphDisabled);
        if (svg.isNull())
            continue;
        icon.addPixmap(svg.pixmap(QSize(16, 16)), spec.mode, spec.state);
    }
    return icon;
}

}   // namespace

// Baked variants, keyed by icon id. applyDockIcons() drops the cache on every
// re-tint, because the variants carry the palette in force when they were made.
static QHash<Qx::DockIcon, QIcon> s_highlightSurfaceIcons;

// The colours the last applyDockIcons() installed. highlightSurfaceIcon() has
// only an id to work from - it is reached from DockTab and the auto hide
// container, which do not carry the set - so the set is kept here.
static DockStyleColors s_activeColors;

QIcon internal::highlightSurfaceIcon(Qx::DockIcon id)
{
    auto it = s_highlightSurfaceIcons.find(id);
    if (it != s_highlightSurfaceIcons.end())
        return *it;
    const QIcon icon = highlightSurfaceIconImpl(s_activeColors.resolved(), id);
    if (!icon.isNull())
        s_highlightSurfaceIcons.insert(id, icon);
    return icon;
}

void internal::resetHighlightSurfaceIconCache()
{
    s_highlightSurfaceIcons.clear();
}

// A button that paints over the highlight fill, not over the window colour:
// the close button of a focused tab (the tab hangs off the highlight), and any
// button on an auto hide container's title bar (that bar is filled with the
// highlight too). Those take the white variants instead of the window text
// ones, or the glyph disappears into the accent.
bool isOnHighlightSurface(const QAbstractButton *button)
{
    if (const DockTab *tab = qobject_cast<const DockTab *>(button->parentWidget()))
        return tab->property("focused").toBool();
    for (const QWidget *w = button->parentWidget(); w; w = w->parentWidget()) {
        if (QLatin1String(w->metaObject()->className()) == QLatin1String("QxDock::DockAutoHideContainer"))
            return true;
    }
    return false;
}

void applyDockIcons(const DockStyleColors &colors, QWidget *root)
{
    s_activeColors = colors.resolved();
    internal::resetHighlightSurfaceIconCache();

    QHash<Qx::DockIcon, QIcon> icons;
    for (const IconSpec &spec : kIconSpecs) {
        QIcon svg = tintedIconForWindow(QLatin1String(spec.file), s_activeColors);
        if (svg.isNull())
            continue;
        icons[spec.id].addPixmap(svg.pixmap(QSize(16, 16)), spec.mode, spec.state);
    }

    DockIconProvider &provider = DockManager::iconProvider();
    for (auto it = icons.constBegin(); it != icons.constEnd(); ++it)
        provider.registerCustomIcon(it.key(), it.value());

    if (root)
        refreshDockIcons(root);
}

void applyDockIcons(const QPalette &palette, QWidget *root)
{
    applyDockIcons(DockStyleColors::fromPalette(palette), root);
}

void refreshDockIcons(QWidget *root)
{
    if (!root)
        return;
    const QList<QWidget *> widgets = root->findChildren<QWidget *>();
    for (QWidget *widget : widgets) {
        QAbstractButton *button = qobject_cast<QAbstractButton *>(widget);
        if (!button)
            continue;
        bool ok = false;
        const int id = button->property("qxdock.iconId").toInt(&ok);
        if (!ok)
            continue;
        QIcon icon = isOnHighlightSurface(button) ? internal::highlightSurfaceIcon(Qx::DockIcon(id))
                                                  : DockManager::iconProvider().customIcon(Qx::DockIcon(id));
        if (!icon.isNull())
            button->setIcon(icon);
    }
}

QX_DOCK_END_NAMESPACE
