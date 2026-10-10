/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef OUTPUTPLUGIN_H
#define OUTPUTPLUGIN_H

#include "qxplugin/qxplugin.h"

#include <QtCore/QObject>

QT_BEGIN_NAMESPACE
class QPlainTextEdit;
QT_END_NAMESPACE

/*!
 * Sample module (Level 1): the output panel.
 *
 * It shows the two smallest things a plugin can do through QxPluginContext -
 * put a widget into a dock (addDock) and speak on the status bar
 * (setStatusMessage) - plus the one thing that lets two modules that share no
 * header work together: it publishes itself into the object pool.
 *
 * filetree reports what it opens here, and it does so without ever seeing this
 * class. A plugin holds no handle to its peers; what it can do is ask the host's
 * pool for an object by name, and output is the object that answers to "output".
 * Publishing is a choice - output makes itself findable, and nothing about a
 * plugin that stays quiet is reachable.
 */
class OutputPlugin : public QxPlugin::QxPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QX_PLUGIN_IID FILE "output.json")
public:
    // The leading :: is not decoration: a class derived from QxPlugin inherits
    // the name QxPlugin, which then hides the namespace of the same name, so
    // QxPlugin::QxPluginContext would look for a member of the class. Every
    // module in here needs it for the same reason. See TOPIC-pitfalls.
    /*!
     * Fills the bottom dock, reports on the status bar, and publishes itself
     * into the object pool under the name "output".
     */
    bool initialize(::QxPlugin::QxPluginContext *context, QString *errorString) override;
    /*! Takes itself back out of the pool; the host outlives the plugin and owns the panel. */
    void shutdown() override;

public Q_SLOTS:
    /*!
     * Appends one line to the panel. No-op before initialize() and after
     * shutdown().
     *
     * A slot rather than a plain method because whoever calls it - filetree, or
     * anyone else that found this object in the pool - does not include this
     * header, so the call has to be reachable by name through the meta-object.
     */
    void appendLine(const QString &line);
private:
    QPlainTextEdit *m_view = Q_NULLPTR;
};

#endif   // OUTPUTPLUGIN_H
