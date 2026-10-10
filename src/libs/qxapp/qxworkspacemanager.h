/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXWORKSPACEMANAGER_H
#define QXWORKSPACEMANAGER_H

#include "qxapp_global.h"

#include "qxcore/qxsettings.h"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>

QX_APP_BEGIN_NAMESPACE

class QxAppShell;
class QxWorkspaceManagerPrivate;

/*!
 * The named layouts of one shell.
 *
 * QxAppShell already persists a layout - geometry, dock state and the visible
 * page - and restores it on the next start. What it cannot do is keep more
 * than one: the arrangement you use while writing code and the one you use
 * while reading output are the same slot, and "give me back what I had" has no
 * name to be called by. This class adds the name.
 *
 * @code
 * QxWorkspaceManager *workspaces = shell.workspaceManager();
 * workspaces->saveWorkspace(tr("Writing"));
 * // ...the user moves the panels around...
 * workspaces->applyWorkspace(tr("Writing"));   // the panels go back
 * @endcode
 *
 * A workspace is the dock layout plus the page that was in front - the two
 * things that make an arrangement what it is. It deliberately does *not*
 * carry the window geometry: geometry belongs to the window, and moving or
 * resizing it because a panel arrangement was picked is a surprise on a
 * maximized or multi-monitor window. Geometry stays with
 * QxAppShell::saveLayout(), which is the one unnamed workspace every
 * application gets for free.
 *
 * The names live in the settings object the shell already uses, so workspaces
 * outlive the process and are shared by every shell reading the same file.
 * Nothing is applied on start-up: whether to restore a workspace, and which
 * one, is the application's call - currentWorkspace() is remembered so it can
 * ask.
 */
class QX_APP_EXPORT QxWorkspaceManager : public QObject
{
    Q_OBJECT
public:
    explicit QxWorkspaceManager(QxAppShell *shell, QObject *parent = Q_NULLPTR);
    ~QxWorkspaceManager() override;

    /*! The shell whose layout this manages; null only for a default-built one. */
    QxAppShell *shell() const;
    /*!
     * The settings object the workspaces are stored in - the one the shell
     * uses, so there is a single configuration file. Null without a shell.
     */
    QX_CORE_PREPEND_NAMESPACE(QxSettings) * settings() const;

    // Reading ---------------------------------------------------------------
    /*! How many workspaces are stored. */
    int count() const;
    /*! Their names, in the order they were saved - the order a menu wants. */
    QStringList workspaceNames() const;
    /*! Whether a workspace called \a name is stored. The name is trimmed. */
    bool contains(const QString &name) const;
    /*!
     * The workspace applied or saved last, empty when there is none.
     *
     * It is read from the settings file rather than kept in memory, so two
     * shells reading the same file agree on it.
     */
    QString currentWorkspace() const;

    // Writing ---------------------------------------------------------------
    /*!
     * Stores the dock layout and the visible page under \a name, which becomes
     * the current workspace. Saving under a name that exists overwrites it.
     *
     * Returns false for an empty name or without a shell.
     */
    bool saveWorkspace(const QString &name);
    /*!
     * Brings back the layout stored under \a name: the dock state is restored
     * and the page that was in front comes to the front again.
     *
     * Returns false when there is no workspace called \a name, when nothing was
     * ever stored under it, or when the stored state no longer applies - the
     * usual reason being that a dock of the arrangement no longer exists.
     */
    bool applyWorkspace(const QString &name);
    /*!
     * Forgets the workspace called \a name. Removing the current one leaves no
     * current workspace behind.
     *
     * Returns false when there is no such workspace.
     */
    bool removeWorkspace(const QString &name);
    /*!
     * Moves everything stored under \a from to \a to, keeping its place in the
     * order. Refuses to overwrite an existing workspace, and reports a rename
     * of a name onto itself as the no-op it is.
     */
    bool renameWorkspace(const QString &from, const QString &to);
    /*! Forgets every workspace. The rest of the settings file is untouched. */
    void clearWorkspaces();

Q_SIGNALS:
    /*! A workspace was stored, first time or overwriting. */
    void workspaceSaved(const QString &name);
    /*! A workspace was brought back on screen. */
    void workspaceApplied(const QString &name);
    /*! A workspace was forgotten - also emitted once per name by clearWorkspaces(). */
    void workspaceRemoved(const QString &name);
    /*! The current workspace changed; \a name is empty when there is none now. */
    void currentWorkspaceChanged(const QString &name);
private:
    Q_DISABLE_COPY(QxWorkspaceManager)
    QX_DECLARE_PRIVATE(QxWorkspaceManager)
};

QX_APP_END_NAMESPACE

#endif   // QXWORKSPACEMANAGER_H
