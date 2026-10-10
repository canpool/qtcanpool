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
 * (setStatusMessage) - and it is the module filetree requires, which is what
 * makes the manager initialize it first.
 *
 * appendLine() is the public API a host calls after the panel is up. filetree
 * has no way to call it: a plugin holds no handle to its peers, so connecting
 * the two is the host's business (the host owns the manager). That is why the
 * method is a slot here rather than a call made from another module.
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
    /*! Fills the bottom dock and reports on the status bar. */
    bool initialize(::QxPlugin::QxPluginContext *context, QString *errorString) override;
    /*! Drops the handle to the panel; the host outlives the plugin and owns it. */
    void shutdown() override;

public Q_SLOTS:
    /*!
     * Appends one line to the panel. No-op before initialize() and after
     * shutdown().
     *
     * A slot rather than a plain method because the host that wires a module to
     * this one does not include module headers - see doc/pages/plugins.md - so
     * the call has to be reachable by name.
     */
    void appendLine(const QString &line);
private:
    QPlainTextEdit *m_view = Q_NULLPTR;
};

#endif   // OUTPUTPLUGIN_H
