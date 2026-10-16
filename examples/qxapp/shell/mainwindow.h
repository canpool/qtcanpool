// Copyright (C) 2026 maminjie <canpool@163.com>
// SPDX-License-Identifier: MulanPSL-2.0

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qxapp/qxappshell.h"

QX_APP_USE_NAMESPACE

// The smallest useful application shell. Everything the ribbon needs to be a
// shell - the Home page, the Layout group on it (Save and Restore, wired to
// saveLayout()/restoreLayout() and the status bar), the "Ready" line and the
// layout stored on close - comes from createStandardShell(); this window only
// adds the groups that are actually its own.
class MainWindow : public QxAppShell
{
public:
    explicit MainWindow(QWidget *parent = nullptr);
};

#endif   // MAINWINDOW_H
