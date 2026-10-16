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

    /*! Ids that never start, whatever their metadata says. Wins over the enable list. */
    void setDisabledPlugins(const QStringList &ids);
    QStringList disabledPlugins() const;

    /*!
     * Ids to start even though their metadata says EnabledByDefault is false.
     * Writing both lists is a contradiction the caller is expected to avoid; when
     * it happens the disabled list wins and the plugin stays off.
     */
    void setEnabledPlugins(const QStringList &ids);
    QStringList enabledPlugins() const;

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

    /*!
     * Discovery, metadata, dependency resolution and ordering, and nothing
     * else. Every plugin that would start is left resolved, waiting for
     * requestPlugin().
     *
     * This is for an application that would rather spend its startup budget on
     * its window than on its modules: the window comes up, and each module is
     * paid for when it is first needed.
     *
     * An alternative to loadPlugins() rather than a step before it: the host
     * reads the metadata once and then either starts everything at once or one
     * plugin at a time. What resolvePlugins() leaves resolved, requestPlugin()
     * can start; calling loadPlugins() afterwards would resolve the same
     * metadata a second time and start a second instance of what is already up.
     */
    void resolvePlugins();
    /*!
     * Starts \a id, along with everything it requires that has not started yet,
     * and returns it - or null when there is no such plugin, when it will not
     * start, or when a dependency of it will not.
     *
     * The plugin goes through exactly the checks a load applies, and reports
     * through the same signals: spec()->error() and errorString() say why when
     * the answer is null.
     *
     * Requires resolvePlugins() (or loadPlugins()) first - before either, the
     * manager has not read the metadata and does not know the id. Asking twice
     * starts nothing twice: an initialized plugin is returned as it is.
     *
     * initialize() runs in dependency order and extensionsInitialized() follows
     * it, as in a load - but this run reaches that second moment with only the
     * plugins that have been asked for so far, since the ones nobody wants yet
     * do not exist. A plugin whose extensionsInitialized() has to see every
     * peer is therefore a reason to call loadPlugins() instead.
     */
    QxPlugin *requestPlugin(const QString &id);

    /*! Calls shutdown() on every initialized plugin, in reverse initialization order. */
    void shutdown();

    /*! Static-build entry (WASM / QTC_STATIC_BUILD): register a pre-built instance factory. */
    void registerStaticPlugin(const QString &key, const QJsonObject &metaData, std::function<QxPlugin *()> factory);

    /*! All specs in dependency order (the order loadPlugins() would load them). */
    QList<QxPluginSpec *> specs() const;
    /*!
     * Every plugin the manager knows about, in discovery order (the static ones
     * first, then the dynamic ones as they were found) - including the ones
     * specs() leaves out: switched off, skipped by default, unresolved or failed.
     * This is the list a manager UI shows, because a plugin that did not start is
     * exactly the one someone opens that UI to look at.
     */
    QList<QxPluginSpec *> allSpecs() const;
    QxPluginSpec *spec(const QString &id) const;
    QxPlugin *plugin(const QString &id) const;

    // Dependency graph ------------------------------------------------------
    /*!
     * The required dependencies of \a id, in declaration order. Empty for an
     * unknown id.
     *
     * These three are answered from the metadata rather than from the outcome
     * of a load, which is the point of them: a plugin manager window has to say
     * who needs whom before anything has been loaded, and the resolution that
     * knows the answer at the end of a load has already changed states by then.
     */
    QStringList dependenciesOf(const QString &id) const;
    /*!
     * The required dependencies of \a id that this manager does not know about
     * at all - the ones that will keep it from starting, sayable before
     * loadPlugins() has run.
     *
     * A dependency that is present but cannot start itself is a different
     * answer, and it is the one loadPlugins() gives: it depends on the run, so
     * it belongs to that call and not to this one.
     */
    QStringList missingDependenciesOf(const QString &id) const;
    /*!
     * The plugins that require \a id - with \a transitive, everything that
     * would be set aside along with them, however deep the chain. This is the
     * closure a load walks when a plugin turns out to be unavailable, which is
     * what a window has to show before it lets anyone switch a plugin off.
     *
     * Only required dependencies are edges. An optional one never blocks, so it
     * is not a reason for anything to be set aside - it does order a run that
     * has both ends, but that is a different question and specs() answers it.
     *
     * \a id itself is never in the result, and the list is sorted and free of
     * duplicates.
     */
    QStringList dependentsOf(const QString &id, bool transitive = false) const;

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
