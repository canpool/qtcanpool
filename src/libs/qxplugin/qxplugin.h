/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXPLUGIN_H
#define QXPLUGIN_H

#include "qxplugin_global.h"

#include <QtCore/QObject>

QX_PLUGIN_BEGIN_NAMESPACE

class QxPluginContext;

/*!
 * Base class of every QtCanpool plugin.
 *
 * A plugin is a QObject whose concrete subclass carries
 * Q_PLUGIN_METADATA(IID QX_PLUGIN_IID FILE "<name>.json") and is loaded by
 * QxPluginManager. The host calls initialize() once every required dependency
 * is up, hands it a QxPluginContext, and expects false (with a reason) when the
 * plugin cannot start; extensionsInitialized() runs once the whole set is up;
 * shutdown() runs in reverse when the host goes down.
 *
 * QxPluginContext is a plugin's only handle: host capabilities are reached
 * through it and nothing else. A plugin has no handle to the manager and none to
 * its peers as objects it may simply drive. Plugins program against
 * QxPluginContext, never against the window class, so renaming the host - a K14
 * concern - never reaches a single plugin.
 *
 * That is a property of the runtime and not only of this comment: a static
 * instance is owned through a private holder rather than parented to the
 * manager, and a dynamic instance is owned by its QPluginLoader and has no
 * parent, so no plugin can walk the object tree back to the manager and from
 * there to its peers.
 *
 * Two plugins still have to be able to cooperate, and the object pool on
 * QxPluginContext is the whole of how: the providing side publishes what it
 * offers, the wanting side looks it up by name or by type, and neither includes
 * the other's header. What that replaces is a host that names both ends and
 * connects them itself - a host that knows no module by name is only possible
 * because the modules can meet without it.
 *
 * @code
 * class MyPlugin : public QxPlugin
 * {
 *     Q_OBJECT
 *     Q_PLUGIN_METADATA(IID QX_PLUGIN_IID FILE "myplugin.json")
 * public:
 *     bool initialize(QxPluginContext *context, QString *errorString) override;
 * };
 * @endcode
 */
class QX_PLUGIN_EXPORT QxPlugin : public QObject
{
    Q_OBJECT
    /*! QxPluginManager sets m_context before initialize(); nothing else should. */
    friend class QxPluginManager;
public:
    explicit QxPlugin(QObject *parent = Q_NULLPTR);
    ~QxPlugin() override;

    /*!
     * Called after every required dependency has been initialized. Take from
     * \a context whatever the plugin needs and return false - filling
     * \a errorString - when it cannot run. A false return removes the plugin and
     * every plugin that depends on it from the run; the host keeps starting.
     */
    virtual bool initialize(QxPluginContext *context, QString *errorString) = 0;

    /*!
     * Called once every plugin is initialized: the place to finish setup that had
     * to wait for the whole set. Peers are reachable here exactly as they are in
     * initialize() - through the object pool on the context - and a plugin that
     * offers something may prefer to publish it here, when it can be sure its own
     * setup is complete. Empty by default.
     *
     * The call runs in the order the plugins were initialized in. That is worth
     * knowing if you care whether a plugin you depend on has reached this point
     * yet; it is not a reverse-order guarantee of the kind Qt Creator promises.
     */
    virtual void extensionsInitialized();

    /*! Called in reverse initialization order on shutdown. Must not fail. */
    virtual void shutdown();

    /*!
     * The plugin interface version this build targets. The host refuses to call
     * initialize() when it is below what the host requires, which is how a 5.0
     * interface change can be rejected cleanly. The first interface is 1.
     */
    virtual int interfaceVersion() const;

    /*! The host context handed to initialize(); null before that call. */
    QxPluginContext *context() const;
protected:
    /*! Set by the manager before initialize(); also reachable through context(). */
    QxPluginContext *m_context = Q_NULLPTR;
};

QX_PLUGIN_END_NAMESPACE

// No Q_DECLARE_INTERFACE here, deliberately: QxPlugin is a QObject subclass, not
// a pure interface, so qobject_cast falls back to the meta-object chain and works
// at any inheritance depth. Declaring it as a Qt interface would instead route
// qobject_cast through qt_metacast(IID), which only succeeds for a class that
// lists Q_INTERFACES(QxPlugin) itself - and moc does not inherit that list, so
// every plugin written the documented way would load but never cast. What ties
// a plugin to this library is the IID in its Q_PLUGIN_METADATA, which is what
// QPluginLoader and the manager check.

#endif   // QXPLUGIN_H
