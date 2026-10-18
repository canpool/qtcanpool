/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockiconprovider.h"
#include "dockmanager.h"

#include <QAbstractButton>
#include <QFile>
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
// tintedIcon() maps that white onto the same colour the black gets, because both
// states are drawn over the window colour. Tinting swaps the shipped colour for
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

QIcon tintedIcon(const QString &fileName, const QPalette &palette, QIcon::Mode mode)
{
    QFile file(QStringLiteral(":/qxdock/res/images/") + fileName);
    if (!file.open(QIODevice::ReadOnly))
        return QIcon();
    QString text = QString::fromUtf8(file.readAll());

    // The white files are the *-focused variants: the same shape as their normal
    // counterpart, drawn in white because they were meant to sit on the highlight
    // fill. Hover and press no longer paint that fill - the sheets use
    // palette(midlight) and palette(mid), which belong to the window colour's
    // family - so on those the glyph has to read like the normal one. Mapping the
    // white to BrightText, as this did, left it #ffffff in *both* schemes
    // (BrightText is the highlight text and that is white in either), which is why
    // a light dock's close button vanished the moment the pointer touched it:
    // the hover fill there is #ffffff too. An icon over the window colour takes
    // the window text colour, whatever mode it is in.
    const QColor glyph = (mode == QIcon::Active || mode == QIcon::Selected)
                             ? palette.color(QPalette::Active, QPalette::WindowText)
                             : palette.color(QPalette::Active, QPalette::BrightText);

    // Replacement order matters: rewrite the six-digit white before the
    // three-digit one, otherwise "#ffffff" would be partially clobbered by the
    // "#fff" rule. The colour is the only thing that changes; the shape is left
    // untouched, so rendering is identical to the original shipped SVG.
    text.replace(QStringLiteral("#ffffff"), glyph.name());
    text.replace(QStringLiteral("#fff"), glyph.name());
    text.replace(QStringLiteral("#a0a0a0"), palette.color(QPalette::Disabled, QPalette::WindowText).name());
    text.replace(QStringLiteral("#000000"), palette.color(QPalette::Active, QPalette::WindowText).name());

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
}   // namespace

void applyDockIcons(const QPalette &palette, QWidget *root)
{
    QHash<Qx::DockIcon, QIcon> icons;
    for (const IconSpec &spec : kIconSpecs) {
        QIcon svg = tintedIcon(QLatin1String(spec.file), palette, spec.mode);
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
        const QIcon icon = DockManager::iconProvider().customIcon(Qx::DockIcon(id));
        if (!icon.isNull())
            button->setIcon(icon);
    }
}

QX_DOCK_END_NAMESPACE
