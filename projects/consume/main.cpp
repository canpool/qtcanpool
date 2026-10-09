/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

// A minimal downstream program: it includes a public header through the
// installed include prefix and links the installed library, nothing else. If
// this compiles and links, the SDK export is complete (headers, targets and
// their Qt dependency are all reachable from find_package alone).

#include "qxapp/qxappshell.h"

#include <QApplication>
#include <QIcon>
#include <QWidget>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QxApp::QxAppShell shell;
    shell.addPage(QStringLiteral("home"), QIcon(), QStringLiteral("Home"), new QWidget());
    shell.addPage(QStringLiteral("log"), QIcon(), QStringLiteral("Log"), new QWidget());

    return shell.pageCount() == 2 ? 0 : 1;
}
