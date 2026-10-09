/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "mainwindow.h"

#include "qxapp/qxsplashscreen.h"

#include <QtCore/QCoreApplication>
#include <QtGui/QPixmap>
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
#endif

    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("qtcanpool"));
    QCoreApplication::setApplicationName(QStringLiteral("AppShellDemo"));

    // The splash reports the start-up work, which here is simply the
    // construction of the shell and the restoration of its layout.
    QxSplashScreen splash(QPixmap(), QStringLiteral("QtCanpool AppShell"), QStringLiteral("3.0"));
    splash.show();
    splash.step(20, QObject::tr("Loading settings..."));

    MainWindow window;
    splash.step(60, QObject::tr("Building the workspace..."));

    window.restoreLayout();
    splash.step(100, QObject::tr("Ready"));
    splash.finish(&window);

    window.show();
    return app.exec();
}
