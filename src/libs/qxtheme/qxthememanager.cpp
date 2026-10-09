/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxthememanager.h"
#include "qxthemepalette.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QHash>
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
public:
    mutable QX_CORE_PREPEND_NAMESPACE(QxSettings) * m_settings;
    Theme m_theme = LightYellow;
    bool m_followSystem = false;
    Theme m_systemLight = LightOffice2013;
    Theme m_systemDark = DarkOfficePlus;
    bool m_warnedMissingStyleSheet = false;
    QHash<int, QString> m_styleSheetFiles;
    QHash<int, QString> m_styleSheetContent;
};

void QxThemeManagerPrivate::start()
{
    ensureSettings();
    reload();
    setupSystemHook();

    if (m_followSystem) {
        m_theme = detectSystemDark() ? m_systemDark : m_systemLight;
    }
    q_ptr->apply();
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
    m_theme = themeFromId(settings->stringValue(QString::fromLatin1(kThemeKey), themeId(m_theme)), m_theme);
    m_followSystem = settings->boolValue(QString::fromLatin1(kFollowKey), m_followSystem);
    m_systemLight =
        themeFromId(settings->stringValue(QString::fromLatin1(kSystemLightKey), themeId(m_systemLight)), m_systemLight);
    m_systemDark =
        themeFromId(settings->stringValue(QString::fromLatin1(kSystemDarkKey), themeId(m_systemDark)), m_systemDark);
    settings->endGroup();
}

void QxThemeManagerPrivate::persistTheme()
{
    QX_CORE_PREPEND_NAMESPACE(QxSettings) *settings = ensureSettings();
    settings->beginGroup(QString::fromLatin1(kGroup));
    settings->setValue(QString::fromLatin1(kThemeKey), themeId(m_theme));
    settings->endGroup();
}

QxThemeManager::QxThemeManager(QObject *parent)
    : QObject(parent)
    , d_ptr(new QxThemeManagerPrivate())
{
    d_ptr->setPublic(this);
    qRegisterMetaType<QX_THEME_PREPEND_NAMESPACE(Theme)>("QxTheme::Theme");
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

Theme QxThemeManager::theme() const
{
    Q_D(const QxThemeManager);
    return d->m_theme;
}

bool QxThemeManager::isDark() const
{
    return isDarkTheme(theme());
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

Theme QxThemeManager::systemLightTheme() const
{
    Q_D(const QxThemeManager);
    return d->m_systemLight;
}

Theme QxThemeManager::systemDarkTheme() const
{
    Q_D(const QxThemeManager);
    return d->m_systemDark;
}

void QxThemeManager::setSystemThemePair(Theme lightTheme, Theme darkTheme)
{
    Q_D(QxThemeManager);
    d->m_systemLight = lightTheme;
    d->m_systemDark = darkTheme;

    QX_CORE_PREPEND_NAMESPACE(QxSettings) *settings = d->ensureSettings();
    settings->beginGroup(QString::fromLatin1(kGroup));
    settings->setValue(QString::fromLatin1(kSystemLightKey), themeId(lightTheme));
    settings->setValue(QString::fromLatin1(kSystemDarkKey), themeId(darkTheme));
    settings->endGroup();

    refreshSystemTheme();
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

QString QxThemeManager::styleSheet(Theme theme) const
{
    Q_D(const QxThemeManager);
    const int key = int(theme);

    const QHash<int, QString>::const_iterator content = d->m_styleSheetContent.constFind(key);
    if (content != d->m_styleSheetContent.constEnd()) {
        return content.value();
    }

    const QHash<int, QString>::const_iterator file = d->m_styleSheetFiles.constFind(key);
    if (file != d->m_styleSheetFiles.constEnd()) {
        QFile reader(file.value());
        if (!reader.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning("QxThemeManager: cannot read style sheet '%s'", qPrintable(file.value()));
            return QString();
        }
        return QString::fromUtf8(reader.readAll());
    }

    return QX_THEME_PREPEND_NAMESPACE(themeStyleSheet)(theme);
}

void QxThemeManager::setStyleSheetFile(Theme theme, const QString &fileName)
{
    Q_D(QxThemeManager);
    d->m_styleSheetContent.remove(int(theme));
    if (fileName.isEmpty()) {
        d->m_styleSheetFiles.remove(int(theme));
    } else {
        d->m_styleSheetFiles.insert(int(theme), fileName);
    }
}

void QxThemeManager::setStyleSheet(Theme theme, const QString &css)
{
    Q_D(QxThemeManager);
    d->m_styleSheetFiles.remove(int(theme));
    if (css.isEmpty()) {
        d->m_styleSheetContent.remove(int(theme));
    } else {
        d->m_styleSheetContent.insert(int(theme), css);
    }
}

void QxThemeManager::setTheme(Theme theme)
{
    Q_D(QxThemeManager);
    d->m_theme = theme;
    d->persistTheme();
    apply();
    Q_EMIT themeChanged(theme);
}

void QxThemeManager::apply()
{
    Q_D(QxThemeManager);
    QApplication *app = qobject_cast<QApplication *>(QCoreApplication::instance());
    if (!app) {
        // The engine is documented to be used after QApplication exists.
        return;
    }

    app->setPalette(QX_THEME_PREPEND_NAMESPACE(palette)(d->m_theme));

    const QString css = styleSheet(d->m_theme);
    if (css.isEmpty() && d->m_theme != Custom) {
        // The built-in style sheet could not be resolved - qxribbon is not part
        // of the process, or the resource prefix was relocated. Installing an
        // empty sheet would silently wipe whatever the application set up, so
        // leave it alone and say so once.
        if (!d->m_warnedMissingStyleSheet) {
            d->m_warnedMissingStyleSheet = true;
            qWarning("QxThemeManager: no style sheet available for theme '%s'; the palette was applied on its own",
                     qPrintable(themeId(d->m_theme)));
        }
        return;
    }
    app->setStyleSheet(css);
}

void QxThemeManager::refreshSystemTheme()
{
    Q_D(QxThemeManager);
    if (!d->m_followSystem) {
        return;
    }
    const Theme target = detectSystemDark() ? d->m_systemDark : d->m_systemLight;
    if (target == d->m_theme) {
        return;
    }
    d->m_theme = target;
    apply();
    Q_EMIT themeChanged(target);
}

QX_THEME_END_NAMESPACE
