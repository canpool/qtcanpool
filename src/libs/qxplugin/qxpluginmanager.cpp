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
    int requiredInterfaceVersion = 1;
    QVersionNumber hostVersion;
    bool hostVersionSet = false;
    QxPluginContext *context = Q_NULLPTR;

    // resolved state, rebuilt every loadPlugins()
    QList<QxPluginSpec *> orderedSpecs;
    QHash<QString, QxPluginSpec *> specById;

    // ownership
    QList<QxPluginSpec *> staticSpecs;
    QList<QxPluginSpec *> dynamicSpecs;
    QList<QPluginLoader *> dynamicLoaders;
    QList<QxPluginManagerPrivate::Entry *> staticEntries;
    QList<QxPluginManagerPrivate::Entry *> dynamicEntries;
    QList<QxPluginManagerPrivate::Entry *> entries;   // static + dynamic, per load

    QString errorString;
    bool hasError = false;

    void gatherDynamic();
    void resolve();
    QString cycleChain(QxPluginSpec *start, const QList<QxPluginSpec *> &live) const;
    QxPlugin *createInstance(QxPluginSpec *spec);
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
        for (const QString &dep : cur->dependencies()) {
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
            s->setState(QxPluginState::Disabled);   // skip, no error
            continue;
        }
        if (!s->isEnabledByDefault()) {
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

    QHash<QString, QList<QxPluginSpec *>> dependents;
    QHash<QString, int> inDegree;
    for (QxPluginSpec *s : live)
        inDegree.insert(s->id(), 0);
    for (QxPluginSpec *s : live) {
        for (const QString &dep : s->dependencies()) {
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
    Q_Q(QxPluginManager);
    for (QxPluginManagerPrivate::Entry *e : entries) {
        if (e->spec != spec)
            continue;
        if (e->isStatic) {
            QxPlugin *instance = e->factory ? e->factory() : Q_NULLPTR;
            // Dynamic plugins are owned by their QPluginLoader; static ones are
            // parented to the manager so they are cleaned up with it.
            if (instance)
                instance->setParent(q);
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

void QxPluginManager::loadPlugins()
{
    Q_D(QxPluginManager);
    d->gatherDynamic();
    d->resolve();

    d->errorString.clear();
    d->hasError = false;

    for (QxPluginSpec *s : std::as_const(d->orderedSpecs)) {
        // a required dep may have failed at load/init time (late cascade)
        bool depFailed = false;
        for (const QString &dep : s->dependencies()) {
            QxPluginSpec *depSpec = d->specById.value(dep);
            if (depSpec && depSpec->state() == QxPluginState::Disabled) {
                s->setState(QxPluginState::Disabled);
                if (s->error().isEmpty())
                    s->setError(
                        QStringLiteral("plugin '%1' depends on '%2', which is not available").arg(s->id(), dep));
                depFailed = true;
                break;
            }
        }
        if (depFailed) {
            emit pluginFailed(s, s->error());
            continue;
        }
        if (s->isDisabled())
            continue;   // skipped by default or by config: nothing to instantiate

        QxPlugin *instance = d->createInstance(s);
        if (!instance) {
            s->setState(QxPluginState::Disabled);
            s->setError(QStringLiteral("plugin '%1' could not be instantiated").arg(s->id()));
            emit pluginFailed(s, s->error());
            continue;
        }
        s->setPlugin(instance);
        s->setState(QxPluginState::Loaded);

        if (instance->interfaceVersion() < d->requiredInterfaceVersion) {
            s->setState(QxPluginState::Disabled);
            s->setError(QStringLiteral("plugin '%1' interface version %2 is below the required %3")
                            .arg(s->id())
                            .arg(instance->interfaceVersion())
                            .arg(d->requiredInterfaceVersion));
            emit pluginFailed(s, s->error());
            continue;
        }

        instance->m_context = d->context;
        QString err;
        if (!instance->initialize(d->context, &err)) {
            s->setState(QxPluginState::Disabled);
            s->setError(err.isEmpty() ? QStringLiteral("plugin '%1' returned false from initialize()").arg(s->id())
                                      : err);
            emit pluginFailed(s, s->error());
            continue;
        }
        s->setState(QxPluginState::Initialized);
        emit pluginInitialized(s);
    }

    // let initialized plugins finish setup now the whole set is up, in the order
    // they were initialized in (they cannot reach each other - the host wires)
    for (QxPluginSpec *s : std::as_const(d->orderedSpecs)) {
        if (s->state() == QxPluginState::Initialized && s->plugin())
            s->plugin()->extensionsInitialized();
    }

    // aggregate failures
    QStringList lines;
    for (QxPluginManagerPrivate::Entry *e : std::as_const(d->entries)) {
        QxPluginSpec *s = e->spec;
        if (!s->error().isEmpty()) {
            lines.append(QStringLiteral("%1: %2").arg(s->id(), s->error()));
            d->hasError = true;
        }
    }
    d->errorString = lines.join(QLatin1String("\n"));
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
