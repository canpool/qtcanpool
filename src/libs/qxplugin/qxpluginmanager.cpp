/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxpluginmanager.h"
#include "qxplugin.h"
#include "qxplugincontext.h"
#include "qxpluginspec.h"

#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonObject>
#include <QtCore/QPluginLoader>
#include <QtCore/QQueue>
#include <QtCore/QSet>

#include <algorithm>
#include <utility>

QX_PLUGIN_BEGIN_NAMESPACE

namespace
{

/*!
 * A plugin library on disk. The same suffixes QLibrary recognises; macOS
 * frameworks are deliberately out of scope - they are bundles, not single
 * objects, and none of our plugins ship that way.
 */
bool isPluginFileName(const QString &name)
{
    return name.endsWith(QLatin1String(".so")) || name.endsWith(QLatin1String(".dll")) ||
           name.endsWith(QLatin1String(".dylib")) || name.endsWith(QLatin1String(".bundle"));
}

const QString kIidKey = QStringLiteral("IID");
const QString kMetaDataKey = QStringLiteral("MetaData");

}   // namespace

class QxPluginManagerPrivate
{
    QX_DECLARE_PUBLIC(QxPluginManager)
public:
    /*! One resolved plugin library (dynamic or static) plus the handle to load it. */
    struct Entry {
        QxPluginSpec *spec = Q_NULLPTR;
        QPluginLoader *loader = Q_NULLPTR;     // dynamic only
        std::function<QxPlugin *()> factory;   // static only
        bool isStatic = false;
    };
    QStringList pluginPaths;
    QSet<QString> disabledPlugins;
    QSet<QString> enabledPlugins;
    int requiredInterfaceVersion = 1;
    QVersionNumber hostVersion;
    bool hostVersionSet = false;
    QxPluginContext *context = Q_NULLPTR;

    // resolved state, rebuilt every loadPlugins()
    QList<QxPluginSpec *> orderedSpecs;
    QHash<QString, QxPluginSpec *> specById;

    // ownership
    /*!
     * Owner of the static plugin instances. Deliberately not the manager: a
     * plugin is handed a QxPluginContext and nothing else, so it must not be
     * able to walk parent() back to the manager and from there reach its peers
     * through plugin()/specs(). A plain QObject member of the private keeps the
     * cleanup with the manager without publishing a way back. Dynamic instances
     * are owned by their QPluginLoader and have no parent at all.
     */
    QObject pluginOwner;
    QList<QxPluginSpec *> staticSpecs;
    QList<QxPluginSpec *> dynamicSpecs;
    QList<QPluginLoader *> dynamicLoaders;
    QList<QxPluginManagerPrivate::Entry *> staticEntries;
    QList<QxPluginManagerPrivate::Entry *> dynamicEntries;
    QList<QxPluginManagerPrivate::Entry *> entries;   // static + dynamic, per load

    QString errorString;
    bool hasError = false;
    /*!
     * Ids whose extensionsInitialized() has been called. Kept because a lazy run
     * reaches that moment more than once - once per request - and the hook is a
     * one-off by contract.
     */
    QSet<QString> extensionsDone;

    void gatherDynamic();
    void resolve();
    QString cycleChain(QxPluginSpec *start, const QList<QxPluginSpec *> &live) const;
    QxPlugin *createInstance(QxPluginSpec *spec);
    /*! Takes one resolved plugin to an initialized instance; see the definition. */
    bool startSpec(QxPluginSpec *spec);
    /*!
     * requestPlugin()'s recursion: \a id and everything it requires, with no
     * announcement of its own. See the definition.
     */
    QxPlugin *requestSpec(const QString &id);
    /*! Calls extensionsInitialized() on every initialized plugin that has not had it yet. */
    void runExtensionsInitialized();
    /*! Recomputes errorString and hasError from the specs. */
    void collectErrors();
};

void QxPluginManagerPrivate::gatherDynamic()
{
    qDeleteAll(dynamicSpecs);
    dynamicSpecs.clear();
    qDeleteAll(dynamicLoaders);
    dynamicLoaders.clear();
    qDeleteAll(dynamicEntries);
    dynamicEntries.clear();
    entries.clear();
    entries.append(staticEntries);

    for (const QString &path : std::as_const(pluginPaths)) {
        QDir dir(path);
        if (!dir.exists())
            continue;
        const QFileInfoList files = dir.entryInfoList(QDir::Files);
        for (const QFileInfo &fi : files) {
            if (!isPluginFileName(fi.fileName()))
                continue;
            auto *loader = new QPluginLoader(fi.absoluteFilePath());
            dynamicLoaders.append(loader);
            const QJsonObject md = loader->metaData();
            if (md.value(kIidKey).toString() != QX_PLUGIN_IID)
                continue;   // not one of ours; leave the loader, it is just skipped
            auto *spec = new QxPluginSpec;
            dynamicSpecs.append(spec);
            if (!spec->read(fi.absoluteFilePath(), md.value(kMetaDataKey).toObject()))
                spec->setState(QxPluginState::Disabled);
            auto *entry = new QxPluginManagerPrivate::Entry{spec, loader, {}, false};
            dynamicEntries.append(entry);
            entries.append(entry);
        }
    }
}

QString QxPluginManagerPrivate::cycleChain(QxPluginSpec *start, const QList<QxPluginSpec *> &live) const
{
    QStringList chain;
    QSet<QString> seen;
    QxPluginSpec *cur = start;
    while (cur && !seen.contains(cur->id())) {
        seen.insert(cur->id());
        chain.append(cur->id());
        QxPluginSpec *next = Q_NULLPTR;
        // Both kinds of edge count here: a cycle that closes through an optional
        // dependency is still a cycle, and naming only half of it would report a
        // chain the reader cannot find in the metadata.
        QStringList edges = cur->dependencies();
        edges += cur->optionalDependencies();
        for (const QString &dep : std::as_const(edges)) {
            QxPluginSpec *depSpec = specById.value(dep);
            // live holds exactly the candidates Kahn could not order; an edge that
            // leaves it points at a node outside the cycle, so it must not be followed.
            // We must not also require depSpec->state() == Read here: step 5 marks each
            // node Disabled right before calling us, and that mutation would otherwise
            // hide the partner's edge and collapse the reported chain (e.g. "a -> b"
            // instead of "a -> b -> a").
            if (live.contains(depSpec)) {
                next = depSpec;
                break;
            }
        }
        cur = next;
    }
    if (cur)
        chain.append(cur->id());   // close the loop at the revisited node
    return chain.join(QStringLiteral(" -> "));
}

void QxPluginManagerPrivate::resolve()
{
    Q_Q(QxPluginManager);
    Q_UNUSED(q)
    orderedSpecs.clear();
    specById.clear();

    // 1. id map + duplicate detection
    for (QxPluginManagerPrivate::Entry *e : std::as_const(entries)) {
        QxPluginSpec *s = e->spec;
        const QString id = s->id();
        if (specById.contains(id)) {
            s->setState(QxPluginState::Disabled);
            s->setError(QStringLiteral("duplicate plugin id '%1'").arg(id));
            continue;
        }
        specById.insert(id, s);
    }

    // 2. base assessment: config-off, skip-by-default, version, missing dep
    for (QxPluginManagerPrivate::Entry *e : std::as_const(entries)) {
        QxPluginSpec *s = e->spec;
        if (s->state() == QxPluginState::Disabled)
            continue;
        if (disabledPlugins.contains(s->id())) {
            s->setState(QxPluginState::Disabled);   // forced off, no error
            continue;
        }
        // A plugin that is off by default only stays off until someone asks for
        // it by name. The forced-off check above still wins, so an id in both
        // lists stays off.
        if (!s->isEnabledByDefault() && !enabledPlugins.contains(s->id())) {
            s->setState(QxPluginState::Disabled);   // skip, no error
            continue;
        }
        if (hostVersionSet) {
            const QVersionNumber lo = s->compatVersion();
            const QVersionNumber hi = s->version();
            if (hostVersion < lo || hostVersion > hi) {
                s->setState(QxPluginState::Disabled);
                s->setError(QStringLiteral("plugin '%1' requires host version in [%2, %3], but the host is %4")
                                .arg(s->id(), lo.toString(), hi.toString(), hostVersion.toString()));
                continue;
            }
        }
        bool missing = false;
        for (const QString &dep : s->dependencies()) {
            if (!specById.contains(dep)) {
                s->setState(QxPluginState::Disabled);
                s->setError(QStringLiteral("plugin '%1' is missing required dependency '%2'").arg(s->id(), dep));
                missing = true;
                break;
            }
        }
        if (missing)
            continue;
        // still a candidate (state Read)
    }

    // 3. candidate availability: a candidate whose required dep is Disabled is not available
    bool changed = true;
    while (changed) {
        changed = false;
        for (QxPluginManagerPrivate::Entry *e : std::as_const(entries)) {
            QxPluginSpec *s = e->spec;
            if (s->state() != QxPluginState::Read)
                continue;
            for (const QString &dep : s->dependencies()) {
                QxPluginSpec *depSpec = specById.value(dep);
                if (depSpec && depSpec->state() == QxPluginState::Disabled) {
                    s->setState(QxPluginState::Disabled);
                    if (s->error().isEmpty())
                        s->setError(
                            QStringLiteral("plugin '%1' depends on '%2', which is not available").arg(s->id(), dep));
                    changed = true;
                    break;
                }
            }
        }
    }

    // 4. Kahn topological sort over the still-live candidates
    QList<QxPluginSpec *> live;
    for (QxPluginManagerPrivate::Entry *e : std::as_const(entries)) {
        QxPluginSpec *s = e->spec;
        if (s->state() == QxPluginState::Read)
            live.append(s);
    }
    const QSet<QxPluginSpec *> liveSet(live.begin(), live.end());

    QHash<QString, QList<QxPluginSpec *>> dependents;
    QHash<QString, int> inDegree;
    for (QxPluginSpec *s : live)
        inDegree.insert(s->id(), 0);
    for (QxPluginSpec *s : live) {
        for (const QString &dep : s->dependencies()) {
            dependents[dep].append(s);
            inDegree[s->id()]++;
        }
        // An optional dependency that is actually there still orders the run -
        // which is what makes it usable at all, since a plugin can only look up
        // what has already published itself. What it never does is block: one
        // that is absent, switched off, or already failed is dropped here, and
        // the dependent starts regardless. Steps 2 and 3 deliberately read only
        // dependencies(), which is where the difference between the two lives.
        for (const QString &dep : s->optionalDependencies()) {
            if (!liveSet.contains(specById.value(dep)))
                continue;
            dependents[dep].append(s);
            inDegree[s->id()]++;
        }
    }

    QQueue<QxPluginSpec *> queue;
    for (QxPluginSpec *s : live)
        if (inDegree.value(s->id()) == 0)
            queue.enqueue(s);

    QList<QxPluginSpec *> order;
    while (!queue.isEmpty()) {
        QxPluginSpec *s = queue.dequeue();
        order.append(s);
        s->setState(QxPluginState::Resolved);
        for (QxPluginSpec *dep : dependents.value(s->id())) {
            inDegree[dep->id()]--;
            if (inDegree.value(dep->id()) == 0)
                queue.enqueue(dep);
        }
    }

    // 5. anything left in live is in a cycle
    if (order.size() < live.size()) {
        for (QxPluginSpec *s : live) {
            if (order.contains(s))
                continue;
            s->setState(QxPluginState::Disabled);
            s->setError(
                QStringLiteral("plugin '%1' is part of a circular dependency: %2").arg(s->id(), cycleChain(s, live)));
        }
    }

    orderedSpecs = order;
}

QxPlugin *QxPluginManagerPrivate::createInstance(QxPluginSpec *spec)
{
    for (QxPluginManagerPrivate::Entry *e : entries) {
        if (e->spec != spec)
            continue;
        if (e->isStatic) {
            QxPlugin *instance = e->factory ? e->factory() : Q_NULLPTR;
            // Dynamic plugins are owned by their QPluginLoader; static ones are
            // parented to a private holder so they are cleaned up with the
            // manager without becoming reachable through parent(). See the note
            // on pluginOwner - this is the difference between the documented
            // "a plugin has no handle to the manager" and a comment.
            if (instance)
                instance->setParent(&pluginOwner);
            return instance;
        }
        if (e->loader)
            return qobject_cast<QxPlugin *>(e->loader->instance());
        return Q_NULLPTR;
    }
    return Q_NULLPTR;
}

// ---------------------------------------------------------------------------

QxPluginManager::QxPluginManager(QObject *parent)
    : QObject(parent)
    , d_ptr(new QxPluginManagerPrivate){QX_INIT_PRIVATE(QxPluginManager)}

    QxPluginManager::~QxPluginManager()
{
    Q_D(QxPluginManager);
    qDeleteAll(d->staticEntries);
    qDeleteAll(d->staticSpecs);
    qDeleteAll(d->dynamicEntries);
    qDeleteAll(d->dynamicSpecs);
    qDeleteAll(d->dynamicLoaders);
    delete d_ptr;
}

void QxPluginManager::setPluginPaths(const QStringList &paths)
{
    Q_D(QxPluginManager);
    d->pluginPaths = paths;
}

QStringList QxPluginManager::pluginPaths() const
{
    Q_D(const QxPluginManager);
    return d->pluginPaths;
}

void QxPluginManager::setDisabledPlugins(const QStringList &ids)
{
    Q_D(QxPluginManager);
    d->disabledPlugins = QSet<QString>(ids.begin(), ids.end());
}

QStringList QxPluginManager::disabledPlugins() const
{
    Q_D(const QxPluginManager);
    return QStringList(d->disabledPlugins.begin(), d->disabledPlugins.end());
}

void QxPluginManager::setEnabledPlugins(const QStringList &ids)
{
    Q_D(QxPluginManager);
    d->enabledPlugins = QSet<QString>(ids.begin(), ids.end());
}

QStringList QxPluginManager::enabledPlugins() const
{
    Q_D(const QxPluginManager);
    return QStringList(d->enabledPlugins.begin(), d->enabledPlugins.end());
}

void QxPluginManager::setRequiredInterfaceVersion(int version)
{
    Q_D(QxPluginManager);
    d->requiredInterfaceVersion = version;
}

int QxPluginManager::requiredInterfaceVersion() const
{
    Q_D(const QxPluginManager);
    return d->requiredInterfaceVersion;
}

void QxPluginManager::setHostVersion(const QVersionNumber &version)
{
    Q_D(QxPluginManager);
    d->hostVersion = version;
    d->hostVersionSet = true;
}

QVersionNumber QxPluginManager::hostVersion() const
{
    Q_D(const QxPluginManager);
    return d->hostVersion;
}

bool QxPluginManager::hasHostVersion() const
{
    Q_D(const QxPluginManager);
    return d->hostVersionSet;
}

void QxPluginManager::setContext(QxPluginContext *context)
{
    Q_D(QxPluginManager);
    d->context = context;
}

QxPluginContext *QxPluginManager::context() const
{
    Q_D(const QxPluginManager);
    return d->context;
}

void QxPluginManager::registerStaticPlugin(const QString &key, const QJsonObject &metaData,
                                           std::function<QxPlugin *()> factory)
{
    Q_D(QxPluginManager);
    auto *spec = new QxPluginSpec;
    d->staticSpecs.append(spec);
    if (!spec->read(key, metaData))
        spec->setState(QxPluginState::Disabled);
    d->staticEntries.append(new QxPluginManagerPrivate::Entry{spec, Q_NULLPTR, factory, true});
}

/*!
 * One step of a load: from a resolved spec to an initialized instance.
 *
 * This is the body of the load loop, kept in one place because a lazy run needs
 * exactly the same steps one plugin at a time - and a plugin started on request
 * has to pass the same checks and report the same failures as one started by
 * loadPlugins(), or "started" would mean two different things. Returns true
 * when the plugin ended up initialized.
 */
bool QxPluginManagerPrivate::startSpec(QxPluginSpec *spec)
{
    Q_Q(QxPluginManager);

    if (spec->state() == QxPluginState::Initialized) {
        return true;
    }

    // a required dep may have failed at load/init time (late cascade)
    for (const QString &dep : spec->dependencies()) {
        QxPluginSpec *depSpec = specById.value(dep);
        if (depSpec && depSpec->state() == QxPluginState::Disabled) {
            spec->setState(QxPluginState::Disabled);
            if (spec->error().isEmpty())
                spec->setError(
                    QStringLiteral("plugin '%1' depends on '%2', which is not available").arg(spec->id(), dep));
            Q_EMIT q->pluginFailed(spec, spec->error());
            return false;
        }
    }
    if (spec->isDisabled()) {
        return false;   // skipped by default or by config: nothing to instantiate
    }

    QxPlugin *instance = createInstance(spec);
    if (!instance) {
        spec->setState(QxPluginState::Disabled);
        spec->setError(QStringLiteral("plugin '%1' could not be instantiated").arg(spec->id()));
        Q_EMIT q->pluginFailed(spec, spec->error());
        return false;
    }
    spec->setPlugin(instance);
    spec->setState(QxPluginState::Loaded);

    if (instance->interfaceVersion() < requiredInterfaceVersion) {
        spec->setState(QxPluginState::Disabled);
        spec->setError(QStringLiteral("plugin '%1' interface version %2 is below the required %3")
                           .arg(spec->id())
                           .arg(instance->interfaceVersion())
                           .arg(requiredInterfaceVersion));
        Q_EMIT q->pluginFailed(spec, spec->error());
        return false;
    }

    instance->m_context = context;
    QString err;
    if (!instance->initialize(context, &err)) {
        spec->setState(QxPluginState::Disabled);
        spec->setError(err.isEmpty() ? QStringLiteral("plugin '%1' returned false from initialize()").arg(spec->id())
                                     : err);
        Q_EMIT q->pluginFailed(spec, spec->error());
        return false;
    }
    spec->setState(QxPluginState::Initialized);
    Q_EMIT q->pluginInitialized(spec);
    return true;
}

/*!
 * requestPlugin()'s recursion. Everything \a id needs is started before \a id
 * itself, which is the only guarantee a plugin can be given: it may take from
 * the pool what has already published itself, and nothing else.
 *
 * Deliberately without the two passes that close a run - extensionsInitialized()
 * and the error roll-up. They belong to the request as a whole, not to each step
 * of it: a dependency that finished before its dependent had even started would
 * be told to extend over a set that is still half-built.
 */
QxPlugin *QxPluginManagerPrivate::requestSpec(const QString &id)
{
    QxPluginSpec *spec = specById.value(id);
    if (!spec) {
        // Either there is no such plugin or resolvePlugins() has not run: both
        // leave nothing to start, and neither is an error of this call.
        return Q_NULLPTR;
    }
    if (spec->state() == QxPluginState::Initialized) {
        return spec->plugin();
    }
    if (spec->state() == QxPluginState::Disabled) {
        return Q_NULLPTR;
    }

    const QStringList required = spec->dependencies();
    for (const QString &dependency : required) {
        requestSpec(dependency);
    }
    return startSpec(spec) ? spec->plugin() : Q_NULLPTR;
}

/*!
 * Lets initialized plugins finish their setup, in the order they were
 * initialized in. Peers are reachable from here exactly as they are from
 * initialize() - through the object pool on the context - so a plugin that
 * offers something may well prefer to publish it here, once its own setup is
 * complete.
 *
 * The hook is a one-off, so the ids it has run for are remembered: a load
 * reaches this moment once, a lazy run reaches it once per request.
 */
void QxPluginManagerPrivate::runExtensionsInitialized()
{
    for (QxPluginSpec *s : std::as_const(orderedSpecs)) {
        if (s->state() != QxPluginState::Initialized || !s->plugin() || extensionsDone.contains(s->id())) {
            continue;
        }
        extensionsDone.insert(s->id());
        s->plugin()->extensionsInitialized();
    }
}

void QxPluginManagerPrivate::collectErrors()
{
    QStringList lines;
    hasError = false;
    for (QxPluginManagerPrivate::Entry *e : std::as_const(entries)) {
        QxPluginSpec *s = e->spec;
        if (!s->error().isEmpty()) {
            lines.append(QStringLiteral("%1: %2").arg(s->id(), s->error()));
            hasError = true;
        }
    }
    errorString = lines.join(QLatin1String("\n"));
}

void QxPluginManager::loadPlugins()
{
    Q_D(QxPluginManager);
    d->gatherDynamic();
    d->resolve();

    d->errorString.clear();
    d->hasError = false;
    d->extensionsDone.clear();

    for (QxPluginSpec *s : std::as_const(d->orderedSpecs)) {
        d->startSpec(s);
    }

    d->runExtensionsInitialized();
    d->collectErrors();
}

void QxPluginManager::resolvePlugins()
{
    Q_D(QxPluginManager);
    d->gatherDynamic();
    d->resolve();

    d->extensionsDone.clear();
    // Resolution is where a missing dependency, a version mismatch and a cycle
    // are found, so the failure report is already due here: a host that only
    // resolves and then shows a plugin list has to be able to say what is wrong
    // with the entries in it without starting anything.
    d->collectErrors();
}

QxPlugin *QxPluginManager::requestPlugin(const QString &id)
{
    Q_D(QxPluginManager);
    QxPlugin *plugin = d->requestSpec(id);
    d->runExtensionsInitialized();
    d->collectErrors();
    return plugin;
}

void QxPluginManager::shutdown()
{
    Q_D(QxPluginManager);
    // reverse initialization order
    QList<QxPluginSpec *> reverse = d->orderedSpecs;
    std::reverse(reverse.begin(), reverse.end());
    for (QxPluginSpec *s : reverse) {
        if (s->state() == QxPluginState::Initialized && s->plugin()) {
            s->plugin()->shutdown();
            s->setState(QxPluginState::Stopped);
        }
    }
}

QList<QxPluginSpec *> QxPluginManager::specs() const
{
    Q_D(const QxPluginManager);
    return d->orderedSpecs;
}

QList<QxPluginSpec *> QxPluginManager::allSpecs() const
{
    Q_D(const QxPluginManager);
    QList<QxPluginSpec *> specs;
    specs.reserve(d->staticEntries.count() + d->dynamicEntries.count());
    for (QxPluginManagerPrivate::Entry *e : d->staticEntries)
        specs.append(e->spec);
    for (QxPluginManagerPrivate::Entry *e : d->dynamicEntries)
        specs.append(e->spec);
    return specs;
}

QxPluginSpec *QxPluginManager::spec(const QString &id) const
{
    Q_D(const QxPluginManager);
    return d->specById.value(id);
}

QxPlugin *QxPluginManager::plugin(const QString &id) const
{
    Q_D(const QxPluginManager);
    QxPluginSpec *s = d->specById.value(id);
    return s ? s->plugin() : Q_NULLPTR;
}

/*!
 * The id -> spec map the graph queries are answered from.
 *
 * Built from allSpecs() rather than read off spec(), which resolves through the
 * map resolve() fills in: before loadPlugins() that map is empty, so every id
 * would look unknown - and "who depends on this" is exactly the question asked
 * before a load.
 */
static QHash<QString, QxPluginSpec *> specIndex(const QList<QxPluginSpec *> &specs)
{
    QHash<QString, QxPluginSpec *> index;
    for (QxPluginSpec *spec : specs) {
        // The first wins, the same way resolve() treats a duplicated id as one
        // plugin and disables the other.
        if (!index.contains(spec->id())) {
            index.insert(spec->id(), spec);
        }
    }
    return index;
}

QStringList QxPluginManager::dependenciesOf(const QString &id) const
{
    const QxPluginSpec *spec = specIndex(allSpecs()).value(id, Q_NULLPTR);
    return spec ? spec->dependencies() : QStringList();
}

QStringList QxPluginManager::missingDependenciesOf(const QString &id) const
{
    const QHash<QString, QxPluginSpec *> index = specIndex(allSpecs());
    const QxPluginSpec *spec = index.value(id, Q_NULLPTR);
    if (!spec) {
        return QStringList();
    }

    QStringList missing;
    for (const QString &dependency : spec->dependencies()) {
        if (!index.contains(dependency)) {
            missing.append(dependency);
        }
    }
    return missing;
}

QStringList QxPluginManager::dependentsOf(const QString &id, bool transitive) const
{
    const QList<QxPluginSpec *> specs = allSpecs();

    QSet<QString> seen;
    seen.insert(id);

    QStringList found;
    QStringList frontier{id};
    while (!frontier.isEmpty()) {
        QStringList next;
        for (QxPluginSpec *spec : specs) {
            if (seen.contains(spec->id())) {
                continue;
            }
            const QStringList dependencies = spec->dependencies();
            for (const QString &dependency : frontier) {
                if (dependencies.contains(dependency)) {
                    seen.insert(spec->id());
                    found.append(spec->id());
                    next.append(spec->id());
                    break;
                }
            }
        }
        if (!transitive) {
            break;
        }
        // One ring further out; a cycle in the metadata simply runs out of
        // plugins to add rather than looping, because seen only grows.
        frontier = next;
    }

    found.sort();
    return found;
}

bool QxPluginManager::hasError() const
{
    Q_D(const QxPluginManager);
    return d->hasError;
}

QString QxPluginManager::errorString() const
{
    Q_D(const QxPluginManager);
    return d->errorString;
}

QX_PLUGIN_END_NAMESPACE
