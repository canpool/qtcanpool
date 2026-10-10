/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXPLUGINMANAGER_H
#define QXPLUGINMANAGER_H

#include "qxplugin_global.h"
#include "qxplugincontext.h"
#include "qxpluginspec.h"

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QVersionNumber>

#include <functional>

QX_PLUGIN_BEGIN_NAMESPACE

class QxPluginManagerPrivate;

/*!
 * Discovers, orders, loads and isolates QtCanpool plugins.
 *
 * Workflow:
 *  - setPluginPaths() / registerStaticPlugin() tell it where plugins live;
 *  - setContext() gives it the QxPluginContext every plugin receives;
 *  - loadPlugins() reads the metadata, topologically sorts by required
 *    dependencies, rejects version mismatches and cycles with a diagnosable
 *    message, then loads and initializes in order. Any single plugin that
 *    fails to load or returns false from initialize() is set aside together
 *    with everything that depends on it - the host keeps starting.
 *
 * Dynamic plugins are found through QPluginLoader; static builds (incl. WASM,
 * K17) use registerStaticPlugin() instead and share the exact same resolution,
 * ordering and failure-isolation logic.
 */
class QX_PLUGIN_EXPORT QxPluginManager : public QObject
{
    Q_OBJECT
public:
    explicit QxPluginManager(QObject *parent = Q_NULLPTR);
    ~QxPluginManager() override;

    void setPluginPaths(const QStringList &paths);
    QStringList pluginPaths() const;

    void setDisabledPlugins(const QStringList &ids);
    QStringList disabledPlugins() const;

    /*! Minimum interface version a plugin must report to be accepted (default 1). */
    void setRequiredInterfaceVersion(int version);
    int requiredInterfaceVersion() const;

    /*! The host version plugins must be compatible with; accept-all until set. */
    void setHostVersion(const QVersionNumber &version);
    QVersionNumber hostVersion() const;
    bool hasHostVersion() const;

    /*! The context handed to every plugin's initialize(); set before loadPlugins(). */
    void setContext(QxPluginContext *context);
    QxPluginContext *context() const;

    /*! Scans the paths (or the registered static plugins), resolves dependencies and loads in order. */
    void loadPlugins();
    /*! Calls shutdown() on every initialized plugin, in reverse initialization order. */
    void shutdown();

    /*! Static-build entry (WASM / QTC_STATIC_BUILD): register a pre-built instance factory. */
    void registerStaticPlugin(const QString &key, const QJsonObject &metaData, std::function<QxPlugin *()> factory);

    /*! All specs in dependency order (the order loadPlugins() would load them). */
    QList<QxPluginSpec *> specs() const;
    QxPluginSpec *spec(const QString &id) const;
    QxPlugin *plugin(const QString &id) const;

    /*! True when at least one plugin failed (load or initialize). Skips-by-default do not count. */
    bool hasError() const;
    /*! Every failure joined once, each prefixed by its plugin id. */
    QString errorString() const;

Q_SIGNALS:
    /*! A plugin reached the Initialized state. */
    void pluginInitialized(QxPluginSpec *spec);
    /*! A plugin was set aside; \a reason is the same string spec()->error() holds. */
    void pluginFailed(QxPluginSpec *spec, const QString &reason);
private:
    QX_DECLARE_PRIVATE(QxPluginManager)
};

QX_PLUGIN_END_NAMESPACE

#endif   // QXPLUGINMANAGER_H
