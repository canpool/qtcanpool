/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXTHEME_H
#define QXTHEME_H

#include "qxtheme_global.h"

#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtGui/QColor>
#include <QtGui/QPalette>

QX_THEME_BEGIN_NAMESPACE

/*!
 * One theme, as the application that owns it describes it.
 *
 * This library is an engine, not a catalogue: it knows nothing about any
 * particular theme. Which themes exist, what they are called, whether they draw
 * on a dark background and what they paint with is what the application says
 * when it registers them. The id is the only field the engine insists on,
 * because the id is what it writes to the configuration file.
 *
 * A theme that carries no style sheet of its own is a legitimate registration:
 * installing it clears the application style sheet and leaves the palette, so
 * an application can offer "no skin" as one of its choices.
 *
 * The palette is deliberately not part of a registration: \a dark says which of
 * the two the engine paints with, lightPalette() or darkPalette(), and a theme
 * that wanted a third one would be describing a platform rather than a look.
 */
class QX_THEME_EXPORT QxThemeInfo
{
public:
    QxThemeInfo();
    QxThemeInfo(const QString &id, const QString &name, bool dark = false);

    /*! True once an id is set, which is the one field that may not be empty. */
    bool isValid() const;

    QString id;         /*!< Stable identifier; written to the configuration. */
    QString name;       /*!< Human readable name, for a settings dialog. */
    bool dark;          /*!< True for a theme that draws on a dark background. */
    QString styleSheet; /*!< The style sheet to install; empty means none. */

    /*!
     * The colour roles the style sheet was built from, keyed the way the sheet
     * names them ("surface", "accent", "text-on-fill", ...).
     *
     * A style sheet only says what a control looks like, and only for the
     * controls it names. A library that paints outside it - qxdock builds its
     * own style sheet and needs the same colours - can otherwise get at these
     * values only by parsing CSS, or by taking whatever the palette happens to
     * hold, which knows about light and dark but not about a theme family.
     * Empty for a theme registered without colours, which is a legitimate
     * registration: such a theme leaves the palette in charge.
     */
    QMap<QString, QColor> colors;
};

/*! The palette behind the light themes: light surfaces, dark text. */
QX_THEME_EXPORT QPalette lightPalette();
/*! The palette behind the dark themes: dark surfaces, light text. */
QX_THEME_EXPORT QPalette darkPalette();
/*! \a dark ? darkPalette() : lightPalette(). */
QX_THEME_EXPORT QPalette paletteFor(bool dark);

/*!
 * Tells the platform which way the application paints: \a dark is the same flag a
 * theme is registered with.
 *
 * A palette and a style sheet describe what the application draws; they say
 * nothing about what the platform draws for it. The windows11 style that Qt 6.7
 * made the default on Windows 11 paints from its own light and dark table picked
 * by the system color scheme and never consults the palette, and the window frame
 * follows the scheme as well - so on a dark desktop a light application ends up
 * with light scroll bars next to a dark title bar, or the other way round,
 * whichever half the theme does not paint itself.
 *
 * From Qt 6.8 an application can ask for a scheme (QStyleHints::setColorScheme),
 * and this is that call spelled with the flag the engine already uses. Below 6.8
 * there is no way to ask and the platform stays in charge, so this does nothing;
 * it also does nothing on platforms whose theme ignores the request.
 *
 * Call it before setPalette(): the colour scheme decides what the platform
 * supplies, the palette decides what replaces it. An application that follows the
 * system rather than pinning a theme wants followSystemColorScheme() instead.
 */
QX_THEME_EXPORT void applyColorScheme(bool dark);

/*!
 * Hands the color scheme back to the platform, undoing applyColorScheme().
 *
 * This is the right call for an application that follows the system
 * (QxThemeManager::setFollowSystemTheme): the platform is already telling the
 * truth there, and a pinned scheme would do more than contradict it - it also
 * stops QStyleHints::colorSchemeChanged from reporting the next system change,
 * which is the signal following is built on.
 *
 * No-op below 6.8, where no scheme can be pinned in the first place.
 */
QX_THEME_EXPORT void followSystemColorScheme();

/*!
 * The accent colour the platform uses, or an invalid QColor when it does not
 * name one.
 *
 * From Qt 6.6 on this is QPalette::Accent, which the platform plugin fills in
 * from the system - the Windows accent, the desktop's selection colour - so
 * that path is one piece of code on every platform. Below 6.6 only Windows
 * keeps the colour somewhere readable, under the DWM key of the registry, and
 * that is the single platform-specific branch here; everywhere else the answer
 * is that the platform does not say.
 *
 * This is what the system offers, not what is in force: a registered theme
 * carries its own highlight colour, and QxThemeManager::accentColor() is the
 * one to ask which of the two is actually being used.
 */
QX_THEME_EXPORT QColor systemAccentColor();

QX_THEME_END_NAMESPACE

#endif   // QXTHEME_H
