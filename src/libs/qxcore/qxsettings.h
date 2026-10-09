/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXSETTINGS_H
#define QXSETTINGS_H

#include "qxcore_global.h"

#include <QtCore/QObject>
#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

#include <functional>

QX_CORE_BEGIN_NAMESPACE

class QxSettingsPrivate;

/**
 * Thin wrapper around QSettings that keeps call sites free of the usual
 * boilerplate: typed accessors with defaults, group handling and a versioned
 * migration hook for layout changes between releases.
 *
 * A QxSettings object owns its QSettings instance, so an application can keep
 * one instance instead of constructing QSettings everywhere; when a migration
 * step runs it always goes through the same object the application reads from.
 */
class QX_CORE_EXPORT QxSettings : public QObject
{
    Q_OBJECT
public:
    /*! A migration step that upgrades the stored layout to a target version. */
    using Migration = std::function<void(QxSettings &settings)>;

    /*!
     * Settings stored in the platform-native location for
     * \a organization / \a application.
     */
    explicit QxSettings(const QString &organization, const QString &application = QString(), QObject *parent = nullptr);
    /*! Settings stored in an explicit file, which keeps tests hermetic. */
    explicit QxSettings(const QString &fileName, QSettings::Format format, QObject *parent = nullptr);
    ~QxSettings() override;

    /*! The underlying QSettings instance, never null. */
    QSettings *settings() const;
    /*! Backing file name, empty for platform-native storage. */
    QString fileName() const;

    // Group -----------------------------------------------------------------
    void beginGroup(const QString &prefix);
    void endGroup();
    QString group() const;

    // Read ------------------------------------------------------------------
    bool contains(const QString &key) const;
    QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;
    QString stringValue(const QString &key, const QString &defaultValue = QString()) const;
    int intValue(const QString &key, int defaultValue = 0) const;
    bool boolValue(const QString &key, bool defaultValue = false) const;
    QStringList stringListValue(const QString &key, const QStringList &defaultValue = QStringList()) const;
    QStringList allKeys() const;

    // Write -----------------------------------------------------------------
    void setValue(const QString &key, const QVariant &value);
    void remove(const QString &key);
    void clear();
    void sync();

    // Migration -------------------------------------------------------------
    /*! Key holding the layout version; "version" by default. */
    void setVersionKey(const QString &key);
    QString versionKey() const;
    int version() const;
    void setVersion(int version);

    /*!
     * Registers a migration that upgrades the layout to \a version. Steps run
     * in ascending version order, so a jump across several versions applies
     * every step in between.
     */
    void addMigration(int version, Migration migration);

    /*!
     * Applies every registered step whose version is greater than the stored
     * version and not greater than \a targetVersion, then stores
     * \a targetVersion. Returns the number of steps that ran, or 0 when the
     * stored version is already at \a targetVersion or beyond.
     */
    int migrate(int targetVersion);

Q_SIGNALS:
    /*!
     * Emitted whenever a value is written or removed. \a key is the
     * group-qualified key, the same one the next read would use.
     */
    void valueChanged(const QString &key, const QVariant &value);
private:
    Q_DISABLE_COPY(QxSettings)
    QX_DECLARE_PRIVATE(QxSettings)
};

QX_CORE_END_NAMESPACE

#endif   // QXSETTINGS_H
