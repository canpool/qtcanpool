/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXPLUGINSPEC_H
#define QXPLUGINSPEC_H

#include "qxplugin_global.h"

#include <QtCore/QJsonObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVersionNumber>

QX_PLUGIN_BEGIN_NAMESPACE

class QxPluginSpecPrivate;

class QxPlugin;

class QxPluginManagerPrivate;

/*!
 * Lifecycle state of a single plugin, as the manager drives it:
 *
 * Invalid   - the metadata could not be read or was malformed
 * Read      - metadata parsed, not yet resolved against the rest
 * Resolved  - every required dependency is present and the version fits
 * Loaded    - the plugin instance was created
 * Initialized - initialize() returned true
 * Stopped   - shutdown() has run
 * Disabled  - not running: forced off, skipped by default, or failed
 */
enum class QxPluginState {
    Invalid,
    Read,
    Resolved,
    Loaded,
    Initialized,
    Stopped,
    Disabled
};

/*!
 * A read-only view of one plugin's metadata, plus the runtime state the manager
 * attaches to it.
 *
 * The manager builds a spec from the plugin.json (dynamic) or from an embedded
 * QJsonObject (static), then walks it through QxPluginState as the run loads.
 * Everything a plugin needs to know about another plugin - its id, its version,
 * its dependency list - lives here; the plugin binary itself is only touched once.
 */
class QX_PLUGIN_EXPORT QxPluginSpec
{
public:
    QxPluginSpec();
    ~QxPluginSpec();

    // Identity --------------------------------------------------------------
    /*! Stable plugin id; also the base name of its library and metadata file. */
    QString id() const;
    QVersionNumber version() const;
    /*! Lowest host version the plugin still supports; the host must be in [compatVersion, version]. */
    QVersionNumber compatVersion() const;
    QString vendor() const;
    QString copyright() const;
    QString category() const;
    QString description() const;
    QString url() const;

    // Dependencies ----------------------------------------------------------
    /*!
     * Required dependency ids, in declaration order. Missing one of these - or
     * getting one that is switched off or failed - takes the plugin down with it.
     */
    QStringList dependencies() const;
    /*!
     * Optional dependency ids (Type == "optional" in the metadata), in
     * declaration order.
     *
     * The difference from dependencies() is entirely about what happens when the
     * other end is not there: an optional dependency that is resolvable is still
     * loaded and initialized first - which is what lets a plugin look up
     * something the other one publishes - but one that is missing, off or failed
     * is quietly dropped, and this plugin starts either way.
     *
     * "test" dependencies are neither: they do not affect load order, and since
     * nothing here runs a plugin's tests in isolation, they are read and ignored.
     */
    QStringList optionalDependencies() const;

    // Enabling --------------------------------------------------------------
    /*! True when the plugin starts unless explicitly disabled (EnabledByDefault). */
    bool isEnabledByDefault() const;
    /*! True when the plugin will not run: forced off, skipped by default, or failed. */
    bool isDisabled() const;

    // Runtime ----------------------------------------------------------------
    QxPluginState state() const;
    /*! The live instance, or null until Loaded. */
    QxPlugin *plugin() const;
    /*! Path of the metadata file (dynamic) or the registration key (static). */
    QString filePath() const;
    /*! Why the plugin is not running; empty when it runs or is only skipped by default. */
    QString error() const;

    // Construction (manager only) ------------------------------------------
    /*! Fills the spec from metadata; returns false (and records error()) on a malformed document. */
    bool read(const QString &filePath, const QJsonObject &metaData);
private:
    friend class QxPluginManager;
    friend class QxPluginManagerPrivate;
    QX_DECLARE_PRIVATE(QxPluginSpec)

    /*! Manager-only state transitions; kept private so only the manager drives the lifecycle. */
    void setState(QxPluginState state);
    void setError(const QString &error);
    void setPlugin(QxPlugin *plugin);
};

QX_PLUGIN_END_NAMESPACE

#endif   // QXPLUGINSPEC_H
