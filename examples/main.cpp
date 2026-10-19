/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "mainwindow.h"

#include <QApplication>

// Only a dock sample has qxdock to link against; add_qtc_example() is what says
// which one this main() is being compiled for.
#ifdef QTC_EXAMPLE_DOCK
#include "qxdock/dockmanager.h"
#include "qxdock/dockstylesheet.h"
QX_DOCK_USE_NAMESPACE
#endif
// ... and only a sample of a themed library builds the theme bundle. The two
// defines are not exclusive: a dock sample is themed and still needs qxdock,
// because the platform's piece of the dock's look is no part of a theme.
#ifdef QTC_EXAMPLE_THEME
#include "themestart.h"
#endif

int main(int argc, char *argv[])
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif

    QApplication a(argc, argv);
#ifdef QTC_EXAMPLE_THEME
    // A sample of qxribbon or qxdock opens on a theme, the way an application
    // would: neither library installs a look of its own. --theme <id> picks
    // another one out of the bundle.
    installExampleTheme();
#elif defined(QTC_EXAMPLE_DOCK)
    // A dock sample puts the library's sheet on the way an application would:
    // qxdock ships none, and the focus flag is read rather than hard-coded
    // because DockTitleBar asks the same question when it reserves room for the
    // focus rule. A themed sample does not need it - a theme carries these
    // rules - so this is the branch for one that is built without a theme.
    a.setStyleSheet(dockStyleSheet(DockManager::testConfigFlag(DockManager::FocusHighlighting)));
#endif
    MainWindow w;
    w.show();
    return a.exec();
}
