/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FILETREEPLUGIN_H
#define FILETREEPLUGIN_H

#include "qxplugin/qxplugin.h"

#include <QtCore/QObject>
#include <QtCore/QString>

QT_BEGIN_NAMESPACE
class QTreeWidget;
QT_END_NAMESPACE

/*!
 * Sample module (Level 2): the file tree.
 *
 * It is the module that has a required dependency: filetree reports what it
 * opens to the output panel, so its metadata requires output and the manager
 * will not initialize it before output is up. The dependency is declared once,
 * in the plugin's PLUGIN_DEPENDS, and lands in the generated metadata - there is
 * no second place for it to drift out of step.
 *
 * What it cannot do is call output itself. A plugin holds no handle to its
 * peers - QxPluginContext is the whole of what it is given - so the module
 * exposes fileActivated() and leaves the connection to the host, which owns the
 * manager and therefore both modules. See doc/pages/plugins.md for why that is
 * the design and not an omission.
 */
class FileTreePlugin : public QxPlugin::QxPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QX_PLUGIN_IID FILE "filetree.json")
public:
    // The leading :: is required: the inherited name QxPlugin hides the
    // namespace of the same name. See the note in outputplugin.h.
    /*! Fills the left dock with a small tree. */
    bool initialize(::QxPlugin::QxPluginContext *context, QString *errorString) override;
    /*! Drops the handle to the panel; the host outlives the plugin and owns it. */
    void shutdown() override;

Q_SIGNALS:
    /*! A file was opened in the tree; the host decides who hears about it. */
    void fileActivated(const QString &path);

public Q_SLOTS:
    /*! Opens \a path as if it had been double-clicked. Lets a host drive the tree by name. */
    void activateFile(const QString &path);
private:
    QTreeWidget *m_view = Q_NULLPTR;
};

#endif   // FILETREEPLUGIN_H
