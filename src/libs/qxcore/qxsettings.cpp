/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "qxsettings.h"

#include <QtCore/QMap>

QX_CORE_BEGIN_NAMESPACE

/*! Mirrors the key resolution QSettings performs on the next read. */
static QString qualifiedKey(const QSettings *settings, const QString &key)
{
    const QString group = settings->group();
    return group.isEmpty() ? key : group + QLatin1Char('/') + key;
}

class QxSettingsPrivate
{
    QX_DECLARE_PUBLIC(QxSettings)
public:
    QxSettingsPrivate()
        : m_settings(nullptr)
        , m_versionKey(QStringLiteral("version"))
    {
    }
public:
    QSettings *m_settings;
    QString m_versionKey;
    QMap<int, QxSettings::Migration> m_migrations;
};

QxSettings::QxSettings(const QString &organization, const QString &application, QObject *parent)
    : QObject(parent)
    , d_ptr(new QxSettingsPrivate())
{
    Q_D(QxSettings);
    d->m_settings = new QSettings(organization, application, this);
}

QxSettings::QxSettings(const QString &fileName, QSettings::Format format, QObject *parent)
    : QObject(parent)
    , d_ptr(new QxSettingsPrivate())
{
    Q_D(QxSettings);
    d->m_settings = new QSettings(fileName, format, this);
}

QxSettings::~QxSettings()
{
    QX_FINI_PRIVATE();
}

QSettings *QxSettings::settings() const
{
    Q_D(const QxSettings);
    return d->m_settings;
}

QString QxSettings::fileName() const
{
    Q_D(const QxSettings);
    return d->m_settings->fileName();
}

// Group ---------------------------------------------------------------------

void QxSettings::beginGroup(const QString &prefix)
{
    Q_D(QxSettings);
    d->m_settings->beginGroup(prefix);
}

void QxSettings::endGroup()
{
    Q_D(QxSettings);
    d->m_settings->endGroup();
}

QString QxSettings::group() const
{
    Q_D(const QxSettings);
    return d->m_settings->group();
}

// Read ----------------------------------------------------------------------

bool QxSettings::contains(const QString &key) const
{
    Q_D(const QxSettings);
    return d->m_settings->contains(key);
}

QVariant QxSettings::value(const QString &key, const QVariant &defaultValue) const
{
    Q_D(const QxSettings);
    return d->m_settings->value(key, defaultValue);
}

QString QxSettings::stringValue(const QString &key, const QString &defaultValue) const
{
    return value(key, defaultValue).toString();
}

int QxSettings::intValue(const QString &key, int defaultValue) const
{
    return value(key, defaultValue).toInt();
}

bool QxSettings::boolValue(const QString &key, bool defaultValue) const
{
    return value(key, defaultValue).toBool();
}

QStringList QxSettings::stringListValue(const QString &key, const QStringList &defaultValue) const
{
    return value(key, defaultValue).toStringList();
}

QStringList QxSettings::allKeys() const
{
    Q_D(const QxSettings);
    return d->m_settings->allKeys();
}

// Write ---------------------------------------------------------------------

void QxSettings::setValue(const QString &key, const QVariant &value)
{
    Q_D(QxSettings);
    d->m_settings->setValue(key, value);
    emit valueChanged(qualifiedKey(d->m_settings, key), value);
}

void QxSettings::remove(const QString &key)
{
    Q_D(QxSettings);
    const QString qualified = qualifiedKey(d->m_settings, key);
    d->m_settings->remove(key);
    emit valueChanged(qualified, QVariant());
}

void QxSettings::clear()
{
    Q_D(QxSettings);
    d->m_settings->clear();
    emit valueChanged(QString(), QVariant());
}

void QxSettings::sync()
{
    Q_D(QxSettings);
    d->m_settings->sync();
}

// Migration -----------------------------------------------------------------

void QxSettings::setVersionKey(const QString &key)
{
    Q_D(QxSettings);
    d->m_versionKey = key;
}

QString QxSettings::versionKey() const
{
    Q_D(const QxSettings);
    return d->m_versionKey;
}

int QxSettings::version() const
{
    Q_D(const QxSettings);
    return d->m_settings->value(d->m_versionKey, 0).toInt();
}

void QxSettings::setVersion(int version)
{
    Q_D(QxSettings);
    d->m_settings->setValue(d->m_versionKey, version);
}

void QxSettings::addMigration(int version, Migration migration)
{
    Q_D(QxSettings);
    d->m_migrations.insert(version, migration);
}

int QxSettings::migrate(int targetVersion)
{
    Q_D(QxSettings);
    const int stored = version();
    if (targetVersion <= stored) {
        return 0;
    }

    int applied = 0;
    for (auto it = d->m_migrations.begin(); it != d->m_migrations.end(); ++it) {
        if (it.key() <= stored || it.key() > targetVersion) {
            continue;
        }
        if (it.value()) {
            it.value()(*this);
            ++applied;
        }
    }

    setVersion(targetVersion);
    return applied;
}

QX_CORE_END_NAMESPACE
