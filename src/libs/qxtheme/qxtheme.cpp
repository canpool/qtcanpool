/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxtheme.h"

#include <QtCore/QCoreApplication>
#include <QtGui/QGuiApplication>
#include <QtGui/QStyleHints>

#if defined(Q_OS_WIN) && QT_VERSION < QT_VERSION_CHECK(6, 6, 0)
// Only the Windows branch below needs these, and only below 6.6: from 6.6 on
// the colour arrives through QPalette::Accent and no system call is made.
#include <QtCore/qt_windows.h>
#endif

QX_THEME_BEGIN_NAMESPACE

QxThemeInfo::QxThemeInfo()
    : dark(false)
{
}

QxThemeInfo::QxThemeInfo(const QString &id, const QString &name, bool dark)
    : id(id)
    , name(name)
    , dark(dark)
{
}

/*!
 * An id is what makes a registration addressable, both for the caller and in
 * the configuration file; every other field is optional.
 */
bool QxThemeInfo::isValid() const
{
    return !id.isEmpty();
}

/*!
 * The Windows accent below Qt 6.6, kept as its own function so the version
 * branch reads as one decision rather than as a block in the middle of another.
 */
#if defined(Q_OS_WIN) && QT_VERSION < QT_VERSION_CHECK(6, 6, 0)
static QColor accentFromRegistry()
{
    HKEY key = Q_NULLPTR;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, LR"(Software\Microsoft\Windows\DWM)", 0, KEY_READ, &key) != ERROR_SUCCESS) {
        return QColor();
    }

    DWORD value = 0;
    DWORD size = sizeof(value);
    DWORD type = 0;
    const LONG status =
        RegQueryValueExW(key, L"AccentColor", Q_NULLPTR, &type, reinterpret_cast<LPBYTE>(&value), &size);
    RegCloseKey(key);
    if (status != ERROR_SUCCESS || type != REG_DWORD || size != sizeof(value)) {
        return QColor();
    }

    // The key holds #AABBGGRR while QColor::fromRgba() reads #AARRGGBB, so the
    // three colour bytes come back in the opposite order; alpha is in its place
    // either way.
    const QColor swapped = QColor::fromRgba(value);
    return QColor::fromRgb(swapped.blue(), swapped.green(), swapped.red(), swapped.alpha());
}
#endif

QColor systemAccentColor()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
    return QGuiApplication::palette().color(QPalette::Accent);
#elif defined(Q_OS_WIN)
    return accentFromRegistry();
#else
    // The desktop's own accent is not readable through Qt below 6.6, and there
    // is no one place the various desktops agree on. Saying so is better than
    // inventing a colour here.
    return QColor();
#endif
}

/*!
 * The color scheme, where the platform lets an application set one.
 *
 * Qt 6.8 is the first version with a setter: QStyleHints::setColorScheme() hands a
 * request to the platform theme, and Qt's own Windows and macOS plugins carry it out -
 * Windows re-reads its palette, repaints the window frame and tells the style, whose
 * windows11 half then draws from the light table instead of the dark one. 6.5-6.7 know
 * the scheme (Qt::ColorScheme, colorSchemeChanged) but cannot override it, so there the
 * platform stays the authority and these two functions are empty.
 */
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
void applyColorScheme(bool dark)
{
    // The hint is read off the application, and the engine is documented to be used
    // after one exists; asking earlier is a no-op here rather than a warning of its own.
    QGuiApplication *app = qobject_cast<QGuiApplication *>(QCoreApplication::instance());
    if (app == nullptr) {
        return;
    }
    app->styleHints()->setColorScheme(dark ? Qt::ColorScheme::Dark : Qt::ColorScheme::Light);
}

void followSystemColorScheme()
{
    QGuiApplication *app = qobject_cast<QGuiApplication *>(QCoreApplication::instance());
    if (app == nullptr) {
        return;
    }
    app->styleHints()->unsetColorScheme();
}
#else
void applyColorScheme(bool dark)
{
    Q_UNUSED(dark);
}

void followSystemColorScheme()
{
}
#endif

QX_THEME_END_NAMESPACE
