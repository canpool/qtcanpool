/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxthemepalette.h"

#include <QtGui/QColor>

QX_THEME_BEGIN_NAMESPACE

namespace
{

/*! The handful of colours a theme needs to describe a complete palette. */
struct Scheme {
    QColor window;
    QColor base;
    QColor alternateBase;
    QColor button;
    QColor text;
    QColor textDisabled;
    QColor highlight;
    QColor highlightText;
    QColor toolTipBase;
    QColor toolTipText;
};

/*!
 * Expands a colour set into a palette. Roles that a style sheet never reaches
 * (frame shadows, menu separators, tooltips) are derived from the window colour
 * so a theme stays internally consistent instead of falling back to Qt's
 * default light values.
 */
QPalette buildPalette(const Scheme &s)
{
    QPalette p;
    p.setColor(QPalette::Window, s.window);
    p.setColor(QPalette::WindowText, s.text);
    p.setColor(QPalette::Base, s.base);
    p.setColor(QPalette::AlternateBase, s.alternateBase);
    p.setColor(QPalette::ToolTipBase, s.toolTipBase);
    p.setColor(QPalette::ToolTipText, s.toolTipText);
    p.setColor(QPalette::Text, s.text);
    p.setColor(QPalette::Button, s.button);
    p.setColor(QPalette::ButtonText, s.text);
    p.setColor(QPalette::BrightText, s.highlightText);
    p.setColor(QPalette::Light, s.window.lighter(130));
    p.setColor(QPalette::Midlight, s.window.lighter(115));
    p.setColor(QPalette::Mid, s.window.darker(115));
    p.setColor(QPalette::Dark, s.window.darker(150));
    p.setColor(QPalette::Shadow, s.window.darker(200));
    p.setColor(QPalette::Highlight, s.highlight);
    p.setColor(QPalette::HighlightedText, s.highlightText);
    p.setColor(QPalette::Link, s.highlight);
    p.setColor(QPalette::LinkVisited, s.highlight.darker(120));
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
    p.setColor(QPalette::PlaceholderText, s.textDisabled);
#endif

    // Disabled group: the style sheets mostly do not cover it, so the palette
    // has to provide readable-but-muted text.
    p.setColor(QPalette::Disabled, QPalette::WindowText, s.textDisabled);
    p.setColor(QPalette::Disabled, QPalette::Text, s.textDisabled);
    p.setColor(QPalette::Disabled, QPalette::ButtonText, s.textDisabled);
    p.setColor(QPalette::Disabled, QPalette::ToolTipText, s.textDisabled);
    p.setColor(QPalette::Disabled, QPalette::HighlightedText, s.textDisabled);
    return p;
}

const Scheme kLightScheme = {
    QColor(0xf5, 0xf5, 0xf5),   // window
    QColor(0xff, 0xff, 0xff),   // base
    QColor(0xf5, 0xf5, 0xf5),   // alternateBase
    QColor(0xf5, 0xf5, 0xf5),   // button
    QColor(0x0f, 0x0f, 0x0f),   // text
    QColor(0xa0, 0xa0, 0xa0),   // textDisabled
    QColor(0x00, 0x78, 0xd4),   // highlight
    QColor(0xff, 0xff, 0xff),   // highlightText
    QColor(0xff, 0xff, 0xff),   // toolTipBase
    QColor(0x0f, 0x0f, 0x0f),   // toolTipText
};

const Scheme kDarkScheme = {
    QColor(0x29, 0x29, 0x29),   // window
    QColor(0x23, 0x28, 0x2c),   // base
    QColor(0x29, 0x29, 0x29),   // alternateBase
    QColor(0x29, 0x29, 0x29),   // button
    QColor(0xf0, 0xf0, 0xf0),   // text
    QColor(0xa0, 0xa0, 0xa0),   // textDisabled
    QColor(0x00, 0x78, 0xd4),   // highlight
    QColor(0xff, 0xff, 0xff),   // highlightText
    QColor(0x23, 0x28, 0x2c),   // toolTipBase
    QColor(0xf0, 0xf0, 0xf0),   // toolTipText
};

}   // namespace

QPalette lightPalette()
{
    return buildPalette(kLightScheme);
}

QPalette darkPalette()
{
    return buildPalette(kDarkScheme);
}

QPalette palette(Theme theme)
{
    return isDarkTheme(theme) ? darkPalette() : lightPalette();
}

QX_THEME_END_NAMESPACE
