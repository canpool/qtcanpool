/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXTEMEMANAGER_H
#define QXTEMEMANAGER_H

#include "qxtheme.h"
#include "qxcore/qxsettings.h"

#include <QtCore/QObject>

QX_THEME_BEGIN_NAMESPACE

class QxThemeManagerPrivate;

/*!
 * Application-wide theme engine.
 *
 * The ribbon library could already swap a style sheet, but nothing owned the
 * decision: no persistence, no palette, no reaction to the operating system
 * switching to dark mode, and no way to re-apply a theme to a window that was
 * already on screen. QxThemeManager closes that gap.
 *
 * Typical use, right after QApplication is constructed:
 * @code
 * QxThemeManager::instance()->setTheme(QxTheme::DarkOfficePlus);
 * @endcode
 *
 * The selection is stored through QxSettings, so the next launch restores it.
 * Switching at run time calls QApplication::setPalette() and
 * QApplication::setStyleSheet(), which is enough for Qt to re-polish every
 * widget: QxRibbon and QxDock widgets pick the change up through their existing
 * QEvent::StyleChange handling.
 *
 * The engine does not link against qxribbon. The built-in style sheets are
 * looked up in the qxribbon resource bundle at run time, which only resolves
 * when that library is part of the process; use setStyleSheetFile() or
 * setStyleSheet() to supply your own.
 */
class QX_THEME_EXPORT QxThemeManager : public QObject
{
    Q_OBJECT
public:
    /*!
     * The process-wide instance, created on first use.
     *
     * Construct it after QApplication, otherwise there is nothing to apply the
     * theme to. The instance is intentionally never destroyed: it outlives
     * QApplication, so tearing it down would only risk a dangling connection.
     */
    static QxThemeManager *instance();

    /*! The theme that is currently applied. */
    Theme theme() const;
    /*! Convenience wrapper around isDarkTheme(theme()). */
    bool isDark() const;

    /*! True when the theme follows the operating system colour scheme. */
    bool followSystemTheme() const;
    /*!
     * Enables or disables following the system colour scheme. Turning it on
     * applies the matching theme immediately and keeps it in sync afterwards.
     * The flag itself is persisted; the theme it derives is not, so the stored
     * value stays the user's explicit choice.
     */
    void setFollowSystemTheme(bool on);

    /*! Theme used while following the system and the system is light. */
    Theme systemLightTheme() const;
    /*! Theme used while following the system and the system is dark. */
    Theme systemDarkTheme() const;
    /*! Overrides the pair used while following the system. Persisted. */
    void setSystemThemePair(Theme lightTheme, Theme darkTheme);

    /*!
     * The settings object backing persistence; created on first use from the
     * application name, and owned by the manager.
     */
    QX_CORE_PREPEND_NAMESPACE(QxSettings) * settings() const;
    /*!
     * Replaces the settings object. Ownership is transferred to the manager.
     * Already stored values are re-read, so this is also the way to point a
     * test at a hermetic file.
     */
    void setSettings(QX_CORE_PREPEND_NAMESPACE(QxSettings) * settings);

    /*! Style sheet that apply() would install for \a theme. */
    QString styleSheet(Theme theme) const;
    /*!
     * Points \a theme at an external style sheet. The registration lives for
     * the lifetime of the process; pass an empty name to fall back to the
     * built-in resource.
     */
    void setStyleSheetFile(Theme theme, const QString &fileName);
    /*!
     * Supplies the style sheet of \a theme inline, which takes precedence over
     * any file. Pass an empty string to drop the override.
     */
    void setStyleSheet(Theme theme, const QString &css);

public Q_SLOTS:
    /*!
     * Applies \a theme, persists it as the explicit choice and emits
     * themeChanged(). Following the system, if enabled, stays enabled and takes
     * over again on the next system change.
     */
    void setTheme(Theme theme);
    /*! Re-installs the palette and style sheet of the current theme. */
    void apply();
    /*!
     * Re-evaluates the system colour scheme. Called automatically on the
     * platforms that report a change; call it manually on platforms that do not
     * (see followSystemTheme()). No-op while following is disabled.
     */
    void refreshSystemTheme();

Q_SIGNALS:
    /*! Emitted after a theme has been applied. */
    void themeChanged(Theme theme);
    /*! Emitted when following the system colour scheme is toggled. */
    void followSystemThemeChanged(bool on);
protected:
    explicit QxThemeManager(QObject *parent = Q_NULLPTR);
    ~QxThemeManager() override;
private:
    Q_DISABLE_COPY(QxThemeManager)
    QX_DECLARE_PRIVATE(QxThemeManager)
};

QX_THEME_END_NAMESPACE

#endif   // QXTEMEMANAGER_H
