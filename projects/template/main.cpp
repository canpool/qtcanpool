/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

// A minimal QtCanpool application.
//
// QxAppShell already provides what every application otherwise rebuilds by
// hand: the ribbon window, the navigation rail, the page stack, a dock area
// around it and a status bar. Replace the page below with your own widget and
// add more pages/docks - see qxapp/qxappshell.h for the full API.
//
// Note that only public headers are included. There is no relative include into
// the QtCanpool source tree, which is the whole point of consuming the SDK.

#include "qxapp/qxappshell.h"

#include <QtCore/QCoreApplication>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>

int main(int argc, char *argv[])
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("qtcanpool"));
    QCoreApplication::setApplicationName(QStringLiteral("qxtemplate"));

    QxApp::QxAppShell shell;
    shell.addPage(QStringLiteral("home"), QIcon(), QObject::tr("Home"), new QLabel(QObject::tr("Hello, QtCanpool!")));
    shell.setStatusMessage(QObject::tr("Ready"));

    // Restore after every page and dock exists: the layout is keyed by page id,
    // so a page added later would have nothing to be restored into.
    shell.restoreLayout();

    shell.show();
    return app.exec();
}
