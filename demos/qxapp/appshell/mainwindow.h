/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qxapp/qxappshell.h"

QT_BEGIN_NAMESPACE
class QMenu;
class QTimer;
QT_END_NAMESPACE

QX_APP_USE_NAMESPACE

/*!
 * The demo window of the shell: a ribbon, a rail of three pages and three
 * docks, with the theme and the layout wired to the framework.
 *
 * It also carries the two 3.3 capabilities, because a capability that is only
 * reachable from a unit test has not been shown to work in a window: the
 * Workspaces group stores and applies named layouts, and the Progress button
 * drives the status bar indicator through a percentage.
 */
class MainWindow : public QxAppShell
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
private:
    void createRibbon();
    void createPages();
    void createDocks();

    QWidget *createOverviewPage();
    QWidget *createEditorPage();
    QWidget *createLogPage();

    QWidget *createExplorerDock();
    QWidget *createPropertiesDock();
    QWidget *createOutputDock();

    // Workspaces -----------------------------------------------------------
    /*! Fills the menu from what the manager says is stored. */
    void refreshWorkspaceMenu();
    void saveWorkspaceAs();
    void forgetWorkspace();
    // Status ---------------------------------------------------------------
    void runProgress();
    void stepProgress();
private:
    QMenu *m_workspaceMenu = nullptr;
    QTimer *m_progressTimer = nullptr;
    int m_progressStep = 0;
};

#endif   // MAINWINDOW_H
