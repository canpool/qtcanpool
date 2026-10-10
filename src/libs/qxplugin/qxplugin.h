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
 * plugin cannot start; extensionsInitialized() is the place for cross-plugin
 * wiring once everyone is up; shutdown() runs in reverse when the host goes down.
 *
 * Plugins program against QxPluginContext, never against the window class, so
 * renaming the host - a K14 concern - never reaches a single plugin.
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

    /*! Called once every plugin is initialized; wire to peers here. Empty by default. */
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

// Let QPluginLoader recognise the interface.
Q_DECLARE_INTERFACE(QxPlugin::QxPlugin, QX_PLUGIN_IID)

#endif   // QXPLUGIN_H
