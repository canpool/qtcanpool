/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qxapp/qxappshell.h"

QX_APP_USE_NAMESPACE

/*!
 * The demo window of the shell: a ribbon, a rail of three pages and three
 * docks, with the theme and the layout wired to the framework.
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
};

#endif   // MAINWINDOW_H
