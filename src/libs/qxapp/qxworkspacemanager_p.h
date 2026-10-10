/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxapp_global.h"
#include "qxworkspacemanager.h"

#include <QtCore/QString>
#include <QtCore/QStringList>

QX_APP_BEGIN_NAMESPACE

/*!
 * The state behind QxWorkspaceManager: the shell it belongs to and the paths
 * the workspaces are written to.
 *
 * Nothing about the workspaces themselves is cached here. The names, the
 * current one and the stored layouts are all read back from the settings
 * object when they are asked for, which is what lets two shells pointed at the
 * same file agree instead of each keeping its own idea of what is stored.
 */
class QxWorkspaceManagerPrivate
{
    QX_DECLARE_PUBLIC(QxWorkspaceManager)
public:
    QxWorkspaceManagerPrivate();
    /*! The settings object of the shell, null when there is no shell. */
    QX_CORE_PREPEND_NAMESPACE(QxSettings) * settings() const;
    /*! The stored names, in the order they were saved. */
    QStringList names() const;
    void setNames(const QStringList &list);
    /*! The name as it is stored and looked up: without surrounding space. */
    static QString normalize(const QString &name);
    /*! The current workspace, read back from the file. */
    QString current() const;
    /*! Writes \a name as the current workspace; an empty one clears the key. */
    void setCurrent(const QString &name);
public:
    QxAppShell *m_shell = Q_NULLPTR;
};

QX_APP_END_NAMESPACE
