/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXTEMEMANAGER_H
#define QXTEMEMANAGER_H

#include "qxtheme.h"
#include "qxcore/qxsettings.h"

#include <QtCore/QObject>
#include <QtCore/QStringList>

QX_THEME_BEGIN_NAMESPACE

class QxThemeManagerPrivate;

/*!
 * Application-wide theme engine.
 *
 * The engine owns the *decision* about a theme - which one is on, that it is
 * remembered, that the palette and the style sheet reach QApplication, that the
 * operating system switching to dark mode is answered - but it does not own any
 * theme. An application registers the themes it wants, each under an id of its
 * own choosing, and addresses them by that id from then on:
 *
 * @code
 * QxThemeManager::instance()->registerTheme({"accent-dark", tr("Accent Dark"), true, css});
 * QxThemeManager::instance()->setTheme(QStringLiteral("accent-dark"));
 * @endcode
 *
 * Nothing is registered until the application registers it, and an id that was
 * never registered is simply not applied: the engine leaves the application as
 * it is rather than clearing a style sheet somebody else installed.
 *
 * The selection is stored through QxSettings, so the next launch restores it.
 * Switching at run time calls QApplication::setPalette() and
 * QApplication::setStyleSheet(), which is enough for Qt to re-polish every
 * widget: QxRibbon and QxDock widgets pick the change up through their existing
 * QEvent::StyleChange handling.
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

    /*!
     * Adds \a info to the set of themes this engine knows, replacing any theme
     * already registered under the same id.
     *
     * An info without an id is ignored. Registering the id that is currently
     * selected installs it at once, because registering after the stored
     * selection was restored is the normal order and waiting for the next
     * setTheme() would leave the window unthemed; no change is announced, since
     * the selection did not change.
     */
    void registerTheme(const QxThemeInfo &info);
    /*! Removes the theme registered under \a id. Selecting it afterwards does nothing. */
    void unregisterTheme(const QString &id);
    /*! True when \a id was registered. */
    bool hasTheme(const QString &id) const;
    /*! Ids of every registered theme, in the order they were registered. */
    QStringList themeIds() const;
    /*! The registration of \a id, or an invalid QxThemeInfo when there is none. */
    QxThemeInfo themeInfo(const QString &id) const;

    /*! Id of the theme that is currently selected; empty while none is. */
    QString theme() const;
    /*! Display name of the theme that is currently selected, empty while none is. */
    QString themeName() const;
    /*! Convenience wrapper: whether the selected theme registers itself as dark. */
    bool isDark() const;

    /*!
     * The accent colour in force: the override when one was set, otherwise what
     * the platform reported when this engine started, otherwise the highlight
     * colour of the current theme. Never an invalid colour, because all three
     * of those are colours a caller can paint with.
     *
     * The platform's answer is taken once, in instance(), before the first
     * apply(): from that point on the application palette belongs to the theme,
     * so asking the platform afterwards would be asking this engine about its
     * own work.
     */
    QColor accentColor() const;
    /*!
     * Overrides the accent colour, for every theme, until it is cleared.
     *
     * An invalid \a color takes the override away and goes back to the
     * platform's colour. Only an override reaches the palette: a theme carries
     * its own highlight colour and was designed around it, so honouring the
     * platform accent by default would repaint all of them.
     */
    void setAccentColor(const QColor &color);
    /*! True while an override is in force, that is while the platform's colour is not the answer. */
    bool hasAccentOverride() const;

    /*! True when the theme follows the operating system colour scheme. */
    bool followSystemTheme() const;
    /*!
     * Enables or disables following the system colour scheme. Turning it on
     * applies the matching theme immediately and keeps it in sync afterwards.
     * The flag itself is persisted; the theme it derives is not, so the stored
     * value stays the user's explicit choice.
     *
     * Following needs a pair to choose from, see setSystemThemePair().
     */
    void setFollowSystemTheme(bool on);

    /*! Theme used while following the system and the system is light. */
    QString systemLightTheme() const;
    /*! Theme used while following the system and the system is dark. */
    QString systemDarkTheme() const;
    /*! Overrides the pair of ids used while following the system. Persisted. */
    void setSystemThemePair(const QString &lightId, const QString &darkId);

    /*!
     * True while the user has asked for less motion.
     *
     * Widgets cannot read the system's reduce-motion preference, so this is the
     * application's own accessibility switch, and the answer every animator
     * gets through motionFor() follows it - a reduced switch turns every tier
     * into a jump to the end state. See qxthememotion.h for the tiers.
     *
     * The choice is persisted, so the next launch starts reduced again.
     */
    bool reduceMotion() const;
    /*!
     * Records the user's ask for less motion, persists it and announces it.
     * Turned on from the application's settings page - the one place that knows
     * what the user asked for.
     */
    void setReduceMotion(bool on);

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

    /*! Style sheet that apply() would install for \a id, empty when none is registered. */
    QString styleSheet(const QString &id) const;
    /*!
     * Replaces the style sheet of the theme registered under \a id, which is
     * how a theme is given its look after the fact - read from a file the
     * application picked, for instance.
     */
    void setStyleSheet(const QString &id, const QString &css);

    /*!
     * Text this engine appends to the theme's own sheet whenever it installs
     * one, empty until the application sets it.
     *
     * An application-level style sheet is one string, so a second one can only
     * replace the first: this engine and a library that dresses its own widgets
     * from a sheet of its own were both writing to QApplication::
     * setStyleSheet(), and whichever ran last won. Putting the two together is
     * what this is for - the theme's sheet first and this text after it, so a
     * rule of the same specificity as one of the theme's is the one that
     * decides, while a rule the theme says nothing about still lands.
     *
     * The text is not persisted and it is not part of a theme: it belongs to
     * the application, which is also what decides when it changes - including
     * rebuilding it from the palette of the theme in force, which is how the
     * piece a theme cannot carry is filled. Nothing is installed while no
     * theme is in force, this text included, because that is what the engine
     * does with the theme's own sheet then (see apply()).
     */
    QString additionalStyleSheet() const;
    /*!
     * Replaces that text and installs the sheet again, so the change is on the
     * widgets by the time the call returns.
     */
    void setAdditionalStyleSheet(const QString &css);

public Q_SLOTS:
    /*!
     * Applies \a id, persists it as the explicit choice and emits
     * themeChanged(). Following the system, if enabled, stays enabled and takes
     * over again on the next system change.
     */
    void setTheme(const QString &id);
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
    void themeChanged(const QString &id);
    /*! Emitted when following the system colour scheme is toggled. */
    void followSystemThemeChanged(bool on);
    /*!
     * Emitted with the colour in force - accentColor(), not the override that
     * was passed in - after setAccentColor() changed it.
     */
    void accentColorChanged(const QColor &color);
    /*! Emitted after setReduceMotion() changed the switch. */
    void reduceMotionChanged(bool on);
protected:
    explicit QxThemeManager(QObject *parent = Q_NULLPTR);
    ~QxThemeManager() override;
private:
    Q_DISABLE_COPY(QxThemeManager)
    QX_DECLARE_PRIVATE(QxThemeManager)
};

QX_THEME_END_NAMESPACE

#endif   // QXTEMEMANAGER_H
