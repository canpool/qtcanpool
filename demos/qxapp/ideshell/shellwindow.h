/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef SHELLWINDOW_H
#define SHELLWINDOW_H

#include "qxapp/qxappshell.h"

#include "qxplugin/qxpluginmanager.h"

#include <QtCore/QString>

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
 * The modules that want to work together find each other in the object pool the
 * shell hands over with the context, so this file names no module even for that:
 * filetree asks the pool for the object called "output" and connects itself.
 * Something that has to be arranged by name between two plugins belongs to the
 * plugins, not to a host that is not supposed to know either of them.
 */
class ShellWindow : public QxAppShell
{
    Q_OBJECT
public:
    explicit ShellWindow(QWidget *parent = nullptr);

    /*!
     * Discovers, orders and loads the plugins, then does the host's part:
     * applies the switches the plugin manager dialog stored and reports what
     * went wrong.
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
     * One line per plugin (id, state, version, error) followed by what the shell
     * ended up with: the pages, the docks and whatever the modules published
     * into the pool. Written for --check: the point of this sample is the
     * composition rather than the pixels, so the composition is what a caller
     * without a screen can read.
     */
    QString composition() const;
private:
    void createRibbon();
    void refreshPageMenu();
    void openPluginManager();
    void announceFailures();

    /*! The directory the manager scans; derived, never configured. */
    static QString pluginDirectory();

    QxPluginManager *m_manager = nullptr;
    QMenu *m_pageMenu = nullptr;
};

#endif   // SHELLWINDOW_H
