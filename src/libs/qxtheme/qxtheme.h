/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXTHEME_H
#define QXTHEME_H

#include "qxtheme_global.h"

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
};

/*! The palette behind the light themes: light surfaces, dark text. */
QX_THEME_EXPORT QPalette lightPalette();
/*! The palette behind the dark themes: dark surfaces, light text. */
QX_THEME_EXPORT QPalette darkPalette();
/*! \a dark ? darkPalette() : lightPalette(). */
QX_THEME_EXPORT QPalette paletteFor(bool dark);

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
