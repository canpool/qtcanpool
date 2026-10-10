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
 * It is the module with a soft dependency: filetree reports what it opens to the
 * output panel, and the tree works perfectly well without it - all that would be
 * lost is the reporting. The metadata therefore declares output as optional
 * (PLUGIN_RECOMMENDS), which buys two things and gives up one: when output is
 * there it is initialized first, when it is not filetree starts anyway, and
 * switching output off no longer takes the file tree down with it.
 *
 * Finding output is the object pool's job. This file includes no header of
 * output's and the two libraries are not linked: the module asks its context for
 * an object named "output" while it initializes, and connects to it through the
 * meta-object. Absent, it says nothing and carries on - see doc/pages/plugins.md
 * for the whole of the arrangement.
 */
class FileTreePlugin : public QxPlugin::QxPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QX_PLUGIN_IID FILE "filetree.json")
public:
    // The leading :: is required: the inherited name QxPlugin hides the
    // namespace of the same name. See the note in outputplugin.h.
    /*! Fills the left dock with a small tree and connects it to the output panel if one is there. */
    bool initialize(::QxPlugin::QxPluginContext *context, QString *errorString) override;
    /*! Drops the connection and the handle to the tree; the host owns the tree. */
    void shutdown() override;

Q_SIGNALS:
    /*! A file was opened in the tree. Nothing in this module decides who hears about it. */
    void fileActivated(const QString &path);

public Q_SLOTS:
    /*! Opens \a path as if it had been double-clicked. Lets a host drive the tree by name. */
    void activateFile(const QString &path);
private:
    QTreeWidget *m_view = Q_NULLPTR;
    /*!
     * The connection to whatever answered to "output", kept so that re-running
     * initialize() replaces it rather than adding another one. See the note in
     * the .cpp: a plugin library outlives the manager that loaded it, so the
     * same instance can be initialized more than once in a process.
     */
    QMetaObject::Connection m_sinkConnection;
};

#endif   // FILETREEPLUGIN_H
