/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "shellwindow.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QTextStream>
#include <QtWidgets/QApplication>

#ifdef Q_OS_WIN
// Windows puts stdout in text mode, which rewrites every '\n' as CRLF. The
// report below is read by a test and by whoever runs --check, so it has to be
// the bytes it says it is - identical on every platform.
#include <fcntl.h>
#include <io.h>
#endif

/*
 * A main() that does not mention a single module - not by type, not by id, not
 * by header. Everything the window ends up holding it got from the metadata of
 * whatever turned up in the plugin directory, which is the whole of the
 * application: five lines of framework and a start().
 */

int main(int argc, char *argv[])
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
#endif

#ifdef Q_OS_WIN
    _setmode(_fileno(stdout), _O_BINARY);
#endif

    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("qtcanpool"));
    QCoreApplication::setApplicationName(QStringLiteral("IdeShellDemo"));

    // --check loads the modules, prints what they composed and leaves without a
    // screen. It is how this demo is verified - the sample is about the
    // composition, not the pixels - and it is the same code the static build
    // runs, which is what makes the K17 answer checkable instead of asserted.
    const bool check = app.arguments().contains(QStringLiteral("--check"));

    ShellWindow window;
    window.start();

    if (check) {
        QTextStream out(stdout);
        out << window.composition() << Qt::endl;
        // Deliberately about the run, not about any module by name: a build with
        // the modules switched off is a legitimate configuration, but one that
        // found nothing at all, or failed somewhere, is not.
        const bool empty = window.pluginManager()->allSpecs().isEmpty();
        return (!empty && !window.pluginManager()->hasError()) ? 0 : 1;
    }

    // After start(), so that the docks the modules contributed are part of what
    // comes back.
    window.restoreLayout();
    window.show();
    return app.exec();
}
