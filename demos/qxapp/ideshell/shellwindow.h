/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef SHELLWINDOW_H
#define SHELLWINDOW_H

#include "qxapp/qxappshell.h"

#include "qxplugin/qxpluginmanager.h"

#include <QtCore/QString>
#include <QtCore/QStringList>

QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE

QX_APP_USE_NAMESPACE
QX_PLUGIN_USE_NAMESPACE

/*!
 * The IDE-style shell: QxAppShell with a plugin manager, and nothing else.
 *
 * This class is the answer to "what does an application that knows none of its
 * modules look like". It includes no module header, links no module library and
 * constructs no module - the pages, the docks and the names in every menu come
 * out of the metadata of whatever the manager found in the plugin directory.
 * Turn all the modules off and the shell still starts; that is the accept
 * criterion, not a fallback.
 *
 * The one thing a host cannot delegate is introducing two plugins to each
 * other: a plugin is handed a QxPluginContext and no handle to its peers, so
 * somebody who owns the manager has to make the connection. That is what
 * kWiring in the .cpp is, and why it is a table of names rather than a call.
 */
class ShellWindow : public QxAppShell
{
    Q_OBJECT
public:
    explicit ShellWindow(QWidget *parent = nullptr);

    /*!
     * Discovers, orders and loads the plugins, then does the host's part:
     * applies the switches the plugin manager dialog stored, connects what the
     * wiring table asks for, and reports what went wrong.
     *
     * Kept out of the constructor because it is the step a caller wants to
     * place itself - the window shows afterwards, the --check run never shows
     * at all - and because loadPlugins() is not something to bury in a
     * constructor's worth of side effects.
     */
    void start();

    /*! The manager behind all of the above; owned by the window. */
    QxPluginManager *pluginManager() const;

    /*!
     * One line per plugin (id, state, version, error) followed by the pages and
     * docks the shell ended up with. Written for --check: the point of this
     * sample is the composition rather than the pixels, so the composition is
     * what a caller without a screen can read.
     */
    QString composition() const;

private:
    void createRibbon();
    void refreshPageMenu();
    void openPluginManager();
    void wirePlugins();
    void announceFailures();

    /*! The directory the manager scans; derived, never configured. */
    static QString pluginDirectory();

    QxPluginManager *m_manager = nullptr;
    QMenu *m_pageMenu = nullptr;
    /*!
     * The wiring rules that could not be applied, in the order they were tried.
     * Kept because a toast is gone in three seconds and --check has no screen:
     * this is the same information in a form a caller can read.
     */
    QStringList m_wireProblems;
};

#endif   // SHELLWINDOW_H
