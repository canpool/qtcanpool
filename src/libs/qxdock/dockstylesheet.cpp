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

/*
 * The names are the theme's, not the dock's: the sheet is one part of a theme
 * and the theme spells a surface, a hairline and the two button states the way
 * its ribbon half does. Anything the dock used to call its own is either one of
 * those (a separator is the border role, a raised surface is the secondary one)
 * or a colour the theme has to be asked for - see the three at the end, which
 * are washes on the accent and a neutral one, and which the generator derives
 * rather than keeps in the palette.
 *
 * tabActiveTop and tabActiveBottom have no token of their own any more: the
 * active tab's gradient runs between the two surfaces, and naming them twice
 * would only give the same answer two ways. The fields stay, because
 * DockStyleColors is published.
 */
const Token kTokens[] = {
    {"surface", &DockStyleColors::surface},
    {"surface-alt", &DockStyleColors::surfaceRaised},
    {"border", &DockStyleColors::divider},
    {"text", &DockStyleColors::text},
    {"text-muted", &DockStyleColors::mutedText},
    {"accent", &DockStyleColors::accent},
    {"accent-text", &DockStyleColors::accentText},
    {"accent-soft", &DockStyleColors::accentSoft},
    // The theme's two accent fills are one colour here: a palette has the
    // highlight and its pair, and no second step down from it, so the fall-back
    // sheet paints an emphasized surface in the same colour it draws the
    // emphasis with. A theme, which has both, does not have to.
    {"accent-fill-strong", &DockStyleColors::accent},
    {"text-on-fill", &DockStyleColors::onAccent},
    {"state-hover", &DockStyleColors::hover},
    {"state-pressed", &DockStyleColors::pressed},
    {"on-accent-hover", &DockStyleColors::onAccentHover},
    {"on-accent-pressed", &DockStyleColors::onAccentPressed},
    {"subtle", &DockStyleColors::subtle},
};

/*
 * The corner tiers, the one non-colour vocabulary a sheet names. The values are
 * the generator's - tools/themes/build_themes.py keeps the table the generated
 * sheets are filled from, and the two stay in step - and base.css is the only
 * fragment that names one, because it is the one file the run time renders as
 * well as the generator.
 */
struct GeometryToken {
    const char *name;
    const char *value;
};

const GeometryToken kGeometryTokens[] = {
    {"corner-none", "0px"}, {"corner-sm", "2px"}, {"corner-md", "3px"}, {"corner-lg", "6px"}, {"corner-pill", "9999px"},
};

/*
 * A style sheet takes #RRGGBB and rgba(r, g, b, a), with the alpha in 0-255.
 * HexArgb (#AARRGGBB) is not used because Qt 5's parser is the older one and
 * this library still builds there.
 */
/*!
 * \a one mixed towards \a other, \a t being how much of \a other to take.
 */
QColor mix(const QColor &one, const QColor &other, double t)
{
    return QColor(qRound(one.red() * (1 - t) + other.red() * t), qRound(one.green() * (1 - t) + other.green() * t),
                  qRound(one.blue() * (1 - t) + other.blue() * t));
}

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
    const DockStyleColors resolved = colors.resolved();
    QString result = fragment;
    for (const Token &token : kTokens) {
        result.replace(QStringLiteral("{{") + QLatin1String(token.name) + QStringLiteral("}}"),
                       colorText(resolved.*(token.member)));
    }
    for (const GeometryToken &token : kGeometryTokens) {
        result.replace(QStringLiteral("{{") + QLatin1String(token.name) + QStringLiteral("}}"),
                       QLatin1String(token.value));
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

    // The wash a button hovers with, which a theme spells as a token of its
    // own: enough accent to read as emphasis, enough surface to stay a hover
    // and not a fill. Derived rather than asked for, because no palette role
    // carries "the accent, but faint".
    colors.accentSoft = mix(colors.accent, window, 0.75);

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

DockStyleColors DockStyleColors::resolved() const
{
    const DockStyleColors fallback = fromPalette(QGuiApplication::palette());
    DockStyleColors colors = *this;
    if (!colors.surface.isValid())
        colors.surface = fallback.surface;
    if (!colors.surfaceRaised.isValid())
        colors.surfaceRaised = fallback.surfaceRaised;
    if (!colors.divider.isValid())
        colors.divider = fallback.divider;
    if (!colors.text.isValid())
        colors.text = fallback.text;
    if (!colors.mutedText.isValid())
        colors.mutedText = fallback.mutedText;
    if (!colors.accent.isValid())
        colors.accent = fallback.accent;
    if (!colors.accentText.isValid())
        colors.accentText = fallback.accentText;
    if (!colors.accentSoft.isValid())
        colors.accentSoft = fallback.accentSoft;
    if (!colors.onAccent.isValid())
        colors.onAccent = fallback.onAccent;
    if (!colors.onAccentHover.isValid())
        colors.onAccentHover = fallback.onAccentHover;
    if (!colors.onAccentPressed.isValid())
        colors.onAccentPressed = fallback.onAccentPressed;
    if (!colors.subtle.isValid())
        colors.subtle = fallback.subtle;
    if (!colors.hover.isValid())
        colors.hover = fallback.hover;
    if (!colors.pressed.isValid())
        colors.pressed = fallback.pressed;
    if (!colors.tabActiveTop.isValid())
        colors.tabActiveTop = fallback.tabActiveTop;
    if (!colors.tabActiveBottom.isValid())
        colors.tabActiveBottom = fallback.tabActiveBottom;
    return colors;
}

/*
 * One fragment, read out of the resource and filled in. A placeholder that
 * survived means the fragment names a token this table does not know; handing
 * that to setStyleSheet() would drop the declaration silently, so the whole
 * fragment is refused instead.
 */
QString renderFragment(const QString &name, const DockStyleColors &colors)
{
    QString result = readFragment(name);
    if (result.isEmpty()) {
        return QString();
    }
    result = render(result, colors);
    if (result.contains(QStringLiteral("{{"))) {
        qWarning("qxdock: the dock style sheet left a placeholder unresolved");
        return QString();
    }
    return result;
}

QString dockStyleSheet(const DockStyleColors &colors, bool focusHighlighting)
{
    // The focus treatment used to be a second fragment, appended or not
    // according to this flag. It is part of base.css now: every one of those
    // rules asks for an attribute that only exists when the flag is on, so the
    // flag decides at match time instead of at assembly time and the parameter
    // no longer changes the string. It is kept rather than dropped because this
    // signature is published, and kept honest by the note in dockstylesheet.h:
    // an application that does not want the focus treatment leaves the flag off,
    // which is what turns both the rules and the focus controller off.
    Q_UNUSED(focusHighlighting);

    // Two fragments in a fixed order: the look, then the pieces an X11 desktop
    // needs. The order is not cosmetic - where the fragment restates a rule
    // instead of adding one, it has to come after the rule it restates.
    QString result = renderFragment(QStringLiteral("base.css"), colors);
    if (result.isEmpty()) {
        return QString();
    }

#if QX_DOCK_X11
    const QString x11 = renderFragment(QStringLiteral("x11.css"), colors);
    if (x11.isEmpty()) {
        return QString();
    }
    result += x11;
#endif

    return result;
}

QString dockStyleSheet(bool focusHighlighting)
{
    return dockStyleSheet(DockStyleColors::fromPalette(QGuiApplication::palette()), focusHighlighting);
}

QString dockPlatformStyleSheet(const DockStyleColors &colors)
{
#if QX_DOCK_X11
    return renderFragment(QStringLiteral("x11.css"), colors);
#else
    Q_UNUSED(colors);
    return QString();
#endif
}

QString dockPlatformStyleSheet()
{
    return dockPlatformStyleSheet(DockStyleColors::fromPalette(QGuiApplication::palette()));
}

QX_DOCK_END_NAMESPACE
