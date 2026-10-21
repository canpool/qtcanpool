/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxthememanager.h"
#include "qxthememotion.h"
#include "qxthemepalette.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QHash>
#include <algorithm>
#include <cmath>
#include <QtCore/QMetaType>
#include <QtGui/QGuiApplication>
#include <QtGui/QPalette>
#include <QtGui/QStyleHints>
#include <QtWidgets/QApplication>

QX_THEME_BEGIN_NAMESPACE

/*! Where settings land when the application never named itself. */
#ifndef QX_THEME_FALLBACK_ORGANIZATION
#define QX_THEME_FALLBACK_ORGANIZATION "qtcanpool"
#endif

namespace
{

const char *const kGroup = "ui";
const char *const kThemeKey = "theme";
const char *const kFollowKey = "followSystemTheme";
const char *const kSystemLightKey = "systemLightTheme";
const char *const kSystemDarkKey = "systemDarkTheme";
const char *const kAccentKey = "accentColor";
const char *const kReduceMotionKey = "reduceMotion";

/*!
 * A colour from the text QSettings stored it as.
 *
 * QColor::fromString() arrived in 6.4 and the QString constructor went away
 * after 6.6, so the two ends of the supported range need a different door into
 * the same parse - and each door is compiled only where it exists.
 */
QColor colorFromText(const QString &text)
{
    if (text.isEmpty()) {
        return QColor();
    }
#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
    return QColor::fromString(text);
#else
    return QColor(text);
#endif
}

namespace
{
// WCAG relative luminance: the channels are linearised before they are mixed,
// because a saturated blue is far darker than its byte values suggest.
double relativeLuminance(const QColor &color)
{
    double channels[3] = {color.redF(), color.greenF(), color.blueF()};
    for (double &c : channels) {
        c = c <= 0.03928 ? c / 12.92 : std::pow((c + 0.055) / 1.055, 2.4);
    }
    return 0.2126 * channels[0] + 0.7152 * channels[1] + 0.0722 * channels[2];
}

double contrast(const QColor &one, const QColor &two)
{
    const double a = relativeLuminance(one);
    const double b = relativeLuminance(two);
    return (std::max(a, b) + 0.05) / (std::min(a, b) + 0.05);
}
}   // namespace

/*!
 * Black or white, whichever stays readable on \a background.
 *
 * White first, and black only when white cannot reach the 3:1 a user interface
 * element is held to. A lightness threshold gives the wrong answer here: it
 * asks which of the two is further from the fill, and on a saturated blue black
 * is - 6.6:1 against white's 3.2:1 - so the accent fill the bundled families
 * emphasize with got black labels while every one of those same themes writes
 * its text-on-fill as white. The pair belongs to the theme; where a theme says
 * nothing, this is the answer the theme would have given.
 */
QColor readableOn(const QColor &background)
{
    return contrast(background, QColor(Qt::white)) >= 3.0 ? QColor(Qt::white) : QColor(Qt::black);
}

/*!
 * Reads the colour scheme the operating system reports.
 *
 * Qt 6.5 is the first version that exposes it. Older releases - and Qt 6 on a
 * platform that does not publish a scheme at all - fall back to the lightness
 * of the application palette, which is what the widgets are painted from
 * anyway.
 */
bool detectSystemDark()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    const Qt::ColorScheme scheme = QGuiApplication::styleHints()->colorScheme();
    if (scheme == Qt::ColorScheme::Dark) {
        return true;
    }
    if (scheme == Qt::ColorScheme::Light) {
        return false;
    }
#endif
    return QGuiApplication::palette().color(QPalette::Window).lightness() < 128;
}

}   // namespace

class QxThemeManagerPrivate
{
    QX_DECLARE_PUBLIC(QxThemeManager)
public:
    QxThemeManagerPrivate()
        : m_settings(nullptr)
    {
    }

    /*! Reads the stored selection and installs the matching theme. */
    void start();
    /*! Subscribes to the platform colour scheme where that is possible. */
    void setupSystemHook();
    /*! Creates the settings object on first use; never returns null. */
    QX_CORE_PREPEND_NAMESPACE(QxSettings) * ensureSettings() const;
    /*! Pulls every persisted value back into the members. */
    void reload();
    /*! Writes the current theme as the explicit choice. */
    void persistTheme();
    /*! Writes the accent override, or removes the key when there is none. */
    void persistAccent();
public:
    mutable QX_CORE_PREPEND_NAMESPACE(QxSettings) * m_settings;
    /*! What the application registered, keyed by id and kept in the order given. */
    QHash<QString, QxThemeInfo> m_themes;
    QStringList m_order;
    /*! Id of the selected theme; empty until one is registered and chosen. */
    QString m_theme;
    bool m_followSystem = false;
    QString m_systemLight;
    QString m_systemDark;
    /*!
     * The platform's accent, taken once before the theme takes the application
     * palette over. Invalid when the platform never named one.
     */
    QColor m_systemAccent;
    /*! Set by setAccentColor(); invalid while the platform's colour is the answer. */
    QColor m_accentOverride;
    bool m_warnedUnknownTheme = false;

    /*! The registration of the selected theme; invalid while there is none. */
    QxThemeInfo current() const
    {
        return m_themes.value(m_theme);
    }
};

void QxThemeManagerPrivate::start()
{
    // Before apply(), and that is the whole point: from the first apply() on, the
    // application palette belongs to the theme, so the platform's accent can
    // only be read while the application still owns it.
    m_systemAccent = QX_THEME_PREPEND_NAMESPACE(systemAccentColor)();

    ensureSettings();
    reload();
    setupSystemHook();

    if (m_followSystem) {
        m_theme = detectSystemDark() ? m_systemDark : m_systemLight;
    }
    // The restored id is remembered and not installed: nothing can have been
    // registered yet, since registering means calling instance() first, so
    // applying here would look the id up in an empty table and warn about a
    // theme that is about to arrive. The registration that supplies it installs
    // it (registerTheme() applies when the id handed to it is the selected one),
    // and so does the first explicit setTheme(). An application that registers
    // nothing therefore never sees a warning about a choice it did make.
    if (m_themes.contains(m_theme)) {
        q_ptr->apply();
    }
}

void QxThemeManagerPrivate::setupSystemHook()
{
    // Only Qt 6.5+ reports the colour scheme as a signal. On older versions the
    // platforms do not offer an equivalent, so refreshSystemTheme() has to be
    // called by the application (for example from a settings dialog).
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    if (QGuiApplication::instance()) {
        QObject::connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged, q_ptr,
                         [this](Qt::ColorScheme) {
                             q_ptr->refreshSystemTheme();
                         });
    }
#endif
}

QX_CORE_PREPEND_NAMESPACE(QxSettings) * QxThemeManagerPrivate::ensureSettings() const
{
    if (!m_settings) {
        QString organization = QCoreApplication::organizationName();
        if (organization.isEmpty()) {
            organization = QString::fromLatin1(QX_THEME_FALLBACK_ORGANIZATION);
        }
        QString application = QCoreApplication::applicationName();
        if (application.isEmpty()) {
            application = organization;
        }
        m_settings = new QX_CORE_PREPEND_NAMESPACE(QxSettings)(organization, application, q_ptr);
    }
    return m_settings;
}

void QxThemeManagerPrivate::reload()
{
    QX_CORE_PREPEND_NAMESPACE(QxSettings) *settings = ensureSettings();
    settings->beginGroup(QString::fromLatin1(kGroup));
    // Ids are what is stored, never a position in a table: a theme the
    // application has not registered yet still restores, and is applied the
    // moment it is registered.
    m_theme = settings->stringValue(QString::fromLatin1(kThemeKey), m_theme);
    m_followSystem = settings->boolValue(QString::fromLatin1(kFollowKey), m_followSystem);
    m_systemLight = settings->stringValue(QString::fromLatin1(kSystemLightKey), m_systemLight);
    m_systemDark = settings->stringValue(QString::fromLatin1(kSystemDarkKey), m_systemDark);
    // Unparsable text is no override, which is also what a fresh installation
    // has: the platform's colour stays the answer.
    m_accentOverride = colorFromText(settings->stringValue(QString::fromLatin1(kAccentKey)));
    // The switch is process-wide and the stored ask is what a fresh launch
    // starts with; a configuration that says nothing leaves it off.
    setMotionReduced(settings->boolValue(QString::fromLatin1(kReduceMotionKey), false));
    settings->endGroup();
}

void QxThemeManagerPrivate::persistAccent()
{
    QX_CORE_PREPEND_NAMESPACE(QxSettings) *settings = ensureSettings();
    settings->beginGroup(QString::fromLatin1(kGroup));
    if (m_accentOverride.isValid()) {
        // HexArgb rather than name(): the alpha is part of the colour, and a
        // name() would drop it silently for every colour that is not opaque.
        settings->setValue(QString::fromLatin1(kAccentKey), m_accentOverride.name(QColor::HexArgb));
    } else {
        settings->remove(QString::fromLatin1(kAccentKey));
    }
    settings->endGroup();
}

void QxThemeManagerPrivate::persistTheme()
{
    QX_CORE_PREPEND_NAMESPACE(QxSettings) *settings = ensureSettings();
    settings->beginGroup(QString::fromLatin1(kGroup));
    settings->setValue(QString::fromLatin1(kThemeKey), m_theme);
    settings->endGroup();
}

QxThemeManager::QxThemeManager(QObject *parent)
    : QObject(parent)
    , d_ptr(new QxThemeManagerPrivate())
{
    d_ptr->setPublic(this);
}

QxThemeManager::~QxThemeManager()
{
    QX_FINI_PRIVATE();
}

QxThemeManager *QxThemeManager::instance()
{
    static QxThemeManager *s_instance = Q_NULLPTR;
    if (!s_instance) {
        s_instance = new QxThemeManager();
        s_instance->d_ptr->start();
    }
    return s_instance;
}

QString QxThemeManager::theme() const
{
    Q_D(const QxThemeManager);
    return d->m_theme;
}

QString QxThemeManager::themeName() const
{
    return themeInfo(theme()).name;
}

bool QxThemeManager::isDark() const
{
    return themeInfo(theme()).dark;
}

void QxThemeManager::registerTheme(const QxThemeInfo &info)
{
    Q_D(QxThemeManager);
    if (!info.isValid()) {
        return;
    }

    if (!d->m_themes.contains(info.id)) {
        d->m_order.append(info.id);
    }
    d->m_themes.insert(info.id, info);

    // Restoring a stored selection happens before anything is registered, so
    // the theme that is already selected is the one to install now.
    if (d->m_theme == info.id) {
        apply();
    }
}

void QxThemeManager::unregisterTheme(const QString &id)
{
    Q_D(QxThemeManager);
    if (!d->m_themes.contains(id)) {
        return;
    }
    d->m_themes.remove(id);
    d->m_order.removeOne(id);
}

bool QxThemeManager::hasTheme(const QString &id) const
{
    Q_D(const QxThemeManager);
    return d->m_themes.contains(id);
}

QStringList QxThemeManager::themeIds() const
{
    Q_D(const QxThemeManager);
    return d->m_order;
}

QxThemeInfo QxThemeManager::themeInfo(const QString &id) const
{
    Q_D(const QxThemeManager);
    return d->m_themes.value(id);
}

QColor QxThemeManager::accentColor() const
{
    Q_D(const QxThemeManager);
    if (d->m_accentOverride.isValid()) {
        return d->m_accentOverride;
    }
    // The theme's own, before the platform's: this has to answer the same
    // question apply() answered when it filled QPalette::Highlight, and that
    // one consults the theme before the platform - a theme that declares an
    // accent and then gets told a different one is a theme that changed its
    // mind halfway through being installed.
    if (d->current().accent.isValid()) {
        return d->current().accent;
    }
    if (d->m_systemAccent.isValid()) {
        return d->m_systemAccent;
    }
    // The theme said nothing about emphasis and nobody overrode it, so the
    // highlight the widgets are using anyway is the accent.
    return QX_THEME_PREPEND_NAMESPACE(paletteFor)(d->current().dark).color(QPalette::Highlight);
}

void QxThemeManager::setAccentColor(const QColor &color)
{
    Q_D(QxThemeManager);
    const QColor override = color.isValid() ? color : QColor();
    if (d->m_accentOverride == override) {
        return;
    }

    d->m_accentOverride = override;
    d->persistAccent();
    // The accent lives in the palette, so installing it is the same call that
    // installs a theme - and it is what makes the override visible.
    apply();
    // The colour in force, not the argument: clearing the override is a change
    // like any other, and what the listeners want is what to paint with now.
    Q_EMIT accentColorChanged(accentColor());
}

bool QxThemeManager::hasAccentOverride() const
{
    Q_D(const QxThemeManager);
    return d->m_accentOverride.isValid();
}

bool QxThemeManager::followSystemTheme() const
{
    Q_D(const QxThemeManager);
    return d->m_followSystem;
}

void QxThemeManager::setFollowSystemTheme(bool on)
{
    Q_D(QxThemeManager);
    if (d->m_followSystem == on) {
        return;
    }
    d->m_followSystem = on;

    QX_CORE_PREPEND_NAMESPACE(QxSettings) *settings = d->ensureSettings();
    settings->beginGroup(QString::fromLatin1(kGroup));
    settings->setValue(QString::fromLatin1(kFollowKey), on);
    settings->endGroup();

    if (!on) {
        // Keep whatever the system selected and record it as the explicit
        // choice, so the next launch does not silently jump to an older value.
        d->persistTheme();
    }
    refreshSystemTheme();
    Q_EMIT followSystemThemeChanged(on);
}

QString QxThemeManager::systemLightTheme() const
{
    Q_D(const QxThemeManager);
    return d->m_systemLight;
}

QString QxThemeManager::systemDarkTheme() const
{
    Q_D(const QxThemeManager);
    return d->m_systemDark;
}

void QxThemeManager::setSystemThemePair(const QString &lightId, const QString &darkId)
{
    Q_D(QxThemeManager);
    d->m_systemLight = lightId;
    d->m_systemDark = darkId;

    QX_CORE_PREPEND_NAMESPACE(QxSettings) *settings = d->ensureSettings();
    settings->beginGroup(QString::fromLatin1(kGroup));
    settings->setValue(QString::fromLatin1(kSystemLightKey), lightId);
    settings->setValue(QString::fromLatin1(kSystemDarkKey), darkId);
    settings->endGroup();

    refreshSystemTheme();
}

bool QxThemeManager::reduceMotion() const
{
    return isMotionReduced();
}

void QxThemeManager::setReduceMotion(bool on)
{
    // The global switch is the state; the manager is where the ask is stored
    // and announced. An application can also call setMotionReduced() directly,
    // and this records and announces the same switch rather than a second one.
    if (isMotionReduced() == on) {
        return;
    }
    setMotionReduced(on);

    QX_CORE_PREPEND_NAMESPACE(QxSettings) *settings = d_func()->ensureSettings();
    settings->beginGroup(QString::fromLatin1(kGroup));
    settings->setValue(QString::fromLatin1(kReduceMotionKey), on);
    settings->endGroup();

    Q_EMIT reduceMotionChanged(on);
}

QX_CORE_PREPEND_NAMESPACE(QxSettings) * QxThemeManager::settings() const
{
    Q_D(const QxThemeManager);
    return d->ensureSettings();
}

void QxThemeManager::setSettings(QX_CORE_PREPEND_NAMESPACE(QxSettings) * settings)
{
    Q_D(QxThemeManager);
    if (d->m_settings == settings) {
        return;
    }
    delete d->m_settings;
    d->m_settings = settings;
    if (d->m_settings) {
        d->m_settings->setParent(this);
    }
    d->reload();
    apply();
}

QString QxThemeManager::styleSheet(const QString &id) const
{
    return themeInfo(id).styleSheet;
}

void QxThemeManager::setStyleSheet(const QString &id, const QString &css)
{
    Q_D(QxThemeManager);
    const QHash<QString, QxThemeInfo>::iterator it = d->m_themes.find(id);
    if (it == d->m_themes.end()) {
        // There is no built-in sheet to fall back on any more, so naming an id
        // that was never registered is a spelling mistake rather than an
        // override of something: leave it alone instead of inventing a theme.
        return;
    }

    it.value().styleSheet = css;
    if (d->m_theme == id) {
        apply();
    }
}

void QxThemeManager::setTheme(const QString &id)
{
    Q_D(QxThemeManager);
    d->m_theme = id;
    d->persistTheme();
    apply();
    Q_EMIT themeChanged(id);
}

void QxThemeManager::apply()
{
    Q_D(QxThemeManager);
    QApplication *app = qobject_cast<QApplication *>(QCoreApplication::instance());
    if (!app) {
        // The engine is documented to be used after QApplication exists.
        return;
    }

    const QxThemeInfo info = d->current();
    if (!info.isValid()) {
        // Nothing is registered under that id, the usual reason being a theme
        // restored from the configuration that this build no longer registers.
        // Installing nothing would silently wipe whatever the application set
        // up, so leave it alone and say so once.
        if (!d->m_warnedUnknownTheme && !d->m_theme.isEmpty()) {
            d->m_warnedUnknownTheme = true;
            qWarning("QxThemeManager: no theme registered under '%s'", qPrintable(d->m_theme));
        }
        return;
    }

    // The tokens a self-drawn control reads at run time follow the theme the
    // way the palette does. The type and spacing tiers a sheet names were
    // filled at generation time, so the one token that still moves at run time
    // is the retime: the theme's scale takes over here, and a theme that says
    // nothing leaves 1.0 in force.
    QX_THEME_PREPEND_NAMESPACE(setMotionScale)(info.motionScale);

    QPalette palette = QX_THEME_PREPEND_NAMESPACE(paletteFor)(info.dark);
    // An override wins over the theme's own: it is the answer to "paint with
    // this", given by the application or by the user, and a theme is a look
    // someone picked. The platform's accent is deliberately *not* consulted
    // here - a theme carries a highlight of its own and was designed around it,
    // so letting the desktop's through by default would repaint every one of
    // them.
    const QColor accent = d->m_accentOverride.isValid() ? d->m_accentOverride : info.accent;
    if (accent.isValid()) {
        // The emphasis is a pair, not a colour: what sits on the fill is
        // HighlightedText, and a glyph on the emphasized surface asks that role
        // and needs it to belong to *this* fill, not to the palette's own
        // highlight. The theme's own pair wins when it registered one - it is
        // the one the sheet was drawn with. An override replaces the fill, so
        // it replaces the pair: a colour the theme never saw has no answer of
        // the theme's to go with it, and readableOn() is the fallback for
        // exactly that, and for a theme that said nothing about emphasis.
        palette.setColor(QPalette::Highlight, accent);
        const QColor onAccent = d->m_accentOverride.isValid() ? readableOn(accent) : info.onAccent;
        palette.setColor(QPalette::HighlightedText, onAccent.isValid() ? onAccent : readableOn(accent));
        // QPalette has no accent text role: the pair of a highlight is
        // HighlightedText, and that is the one set above, so the accent role is
        // the only one to add - and it only exists from 6.6 on.
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette.setColor(QPalette::Accent, accent);
#endif
    }
    // The scheme has to be settled before the palette: it decides what the platform
    // paints for itself (the windows11 style on Windows 11 draws from its own light or
    // dark table and never reads the palette, and the window frame follows the scheme
    // too), while the palette decides what replaces the platform's colours. Following
    // the system is the one case where the platform is already right - and pinning a
    // scheme there would also silence colorSchemeChanged, which is what tells this class
    // to switch, so that case hands the scheme back instead.
    if (d->m_followSystem) {
        QX_THEME_PREPEND_NAMESPACE(followSystemColorScheme)();
    } else {
        QX_THEME_PREPEND_NAMESPACE(applyColorScheme)(info.dark);
    }
    app->setPalette(palette);

    // An empty sheet is a theme that paints nothing of its own, not a theme
    // that failed to resolve: installing it is what takes the previous theme's
    // sheet back off the widgets.
    app->setStyleSheet(info.styleSheet);
}

void QxThemeManager::refreshSystemTheme()
{
    Q_D(QxThemeManager);
    if (!d->m_followSystem) {
        return;
    }
    const QString target = detectSystemDark() ? d->m_systemDark : d->m_systemLight;
    if (target == d->m_theme) {
        return;
    }
    d->m_theme = target;
    apply();
    Q_EMIT themeChanged(target);
}

QX_THEME_END_NAMESPACE
