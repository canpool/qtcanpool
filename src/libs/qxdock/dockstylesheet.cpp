/**
 * Copyleft (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockstylesheet.h"

#include <QDebug>
#include <QFile>
#include <QGuiApplication>
#include <QTextStream>

QX_DOCK_BEGIN_NAMESPACE

namespace
{

/*
 * Reads one fragment out of res/dockstyle.qrc and returns its text, or an empty
 * string (with a word on stderr) when the resource is not there. Every caller
 * treats that as a broken build rather than as "no styling".
 */
QString readFragment(const QString &name)
{
    // The prefix belongs to res/dockstyle.qrc, which the application builds in.
    const QString fileName = QStringLiteral(":/dockstyle/") + name;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("qxdock: cannot open the dock style sheet %s", qPrintable(fileName));
        return QString();
    }
    const QString result = QTextStream(&file).readAll();
    file.close();
    return result;
}

/*! One placeholder in the sheets and the field that fills it. */
struct Token {
    const char *name;
    QColor DockStyleColors::*member;
};

const Token kTokens[] = {
    {"surface", &DockStyleColors::surface},
    {"surface-raised", &DockStyleColors::surfaceRaised},
    {"divider", &DockStyleColors::divider},
    {"text", &DockStyleColors::text},
    {"muted-text", &DockStyleColors::mutedText},
    {"accent", &DockStyleColors::accent},
    {"accent-text", &DockStyleColors::accentText},
    {"on-accent", &DockStyleColors::onAccent},
    {"on-accent-hover", &DockStyleColors::onAccentHover},
    {"on-accent-pressed", &DockStyleColors::onAccentPressed},
    {"subtle", &DockStyleColors::subtle},
    {"hover", &DockStyleColors::hover},
    {"pressed", &DockStyleColors::pressed},
    {"tab-active-top", &DockStyleColors::tabActiveTop},
    {"tab-active-bottom", &DockStyleColors::tabActiveBottom},
};

/*
 * A style sheet takes #RRGGBB and rgba(r, g, b, a), with the alpha in 0-255.
 * HexArgb (#AARRGGBB) is not used because Qt 5's parser is the older one and
 * this library still builds there.
 */
QString colorText(const QColor &color)
{
    if (color.alpha() == 255) {
        return color.name(QColor::HexRgb);
    }
    return QStringLiteral("rgba(%1, %2, %3, %4)")
        .arg(color.red())
        .arg(color.green())
        .arg(color.blue())
        .arg(color.alpha());
}

/*!
 * Substitutes every placeholder. A field the caller left invalid takes the value
 * fromPalette() gives for it, so a partial mapping still produces a complete
 * sheet; Qt drops a declaration whose value is empty without saying so.
 */
QString render(const QString &fragment, const DockStyleColors &colors)
{
    const DockStyleColors fallback = DockStyleColors::fromPalette(QGuiApplication::palette());
    QString result = fragment;
    for (const Token &token : kTokens) {
        const QColor chosen = colors.*(token.member);
        const QColor value = chosen.isValid() ? chosen : fallback.*(token.member);
        result.replace(QStringLiteral("{{") + QLatin1String(token.name) + QStringLiteral("}}"), colorText(value));
    }
    return result;
}

}   // namespace

DockStyleColors DockStyleColors::fromPalette(const QPalette &palette)
{
    DockStyleColors colors;

    const QColor window = palette.color(QPalette::Window);
    const QColor onAccent = palette.color(QPalette::HighlightedText);
    const bool darkWindow = window.lightness() < 128;

    colors.surface = window;
    colors.surfaceRaised = palette.color(QPalette::Light);
    // A separator has to move away from the surface, and which way that is
    // depends on the surface: darker than a light window, lighter than a dark
    // one. The old sheets used palette(dark) for the rules and palette(light)
    // for a tab's vertical line, so the same idea came out in two opposite
    // directions - and the tab line all but vanished in a light scheme.
    colors.divider = palette.color(darkWindow ? QPalette::Light : QPalette::Dark);
    colors.text = palette.color(QPalette::WindowText);
    colors.mutedText = palette.color(QPalette::PlaceholderText);
    colors.accent = palette.color(QPalette::Highlight);
    colors.accentText = colors.accent;
    colors.onAccent = onAccent;

    QColor onAccentHover = onAccent;
    onAccentHover.setAlpha(60);
    colors.onAccentHover = onAccentHover;

    QColor onAccentPressed = onAccent;
    onAccentPressed.setAlpha(92);
    colors.onAccentPressed = onAccentPressed;

    // A black wash on a dark surface is four levels of difference, which is the
    // same as none; the side tab marks have to step the other way there.
    colors.subtle = darkWindow ? QColor(255, 255, 255, 48) : QColor(0, 0, 0, 48);

    colors.hover = palette.color(QPalette::Light);
    colors.pressed = palette.color(QPalette::Mid);
    colors.tabActiveTop = window;
    colors.tabActiveBottom = palette.color(QPalette::Light);
    return colors;
}

QString dockStyleSheet(const DockStyleColors &colors, bool focusHighlighting)
{
    // Three fragments in a fixed order: the look, then the two modifiers. The
    // order is not cosmetic - where a modifier restates a rule instead of adding
    // one, it has to come after the rule it restates.
    QString result = readFragment(QStringLiteral("base.css"));
    if (result.isEmpty()) {
        return QString();
    }

    if (focusHighlighting) {
        const QString focus = readFragment(QStringLiteral("focus.css"));
        if (focus.isEmpty()) {
            return QString();
        }
        result += focus;
    }

#if QX_DOCK_X11
    const QString x11 = readFragment(QStringLiteral("x11.css"));
    if (x11.isEmpty()) {
        return QString();
    }
    result += x11;
#endif

    result = render(result, colors);

    // A placeholder that survived means a fragment names a token this function
    // does not know. Handing that to setStyleSheet() would drop the declaration
    // silently, so the whole sheet is refused instead.
    if (result.contains(QStringLiteral("{{"))) {
        qWarning("qxdock: the dock style sheet left a placeholder unresolved");
        return QString();
    }
    return result;
}

QString dockStyleSheet(bool focusHighlighting)
{
    return dockStyleSheet(DockStyleColors::fromPalette(QGuiApplication::palette()), focusHighlighting);
}

QX_DOCK_END_NAMESPACE
