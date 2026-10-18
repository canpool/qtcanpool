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

int main(int argc, char *argv[])
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif

    QApplication a(argc, argv);
#ifdef QTC_EXAMPLE_DOCK
    // A dock sample puts the library's sheet on the way an application would:
    // qxdock ships none, and the focus flag is read rather than hard-coded
    // because DockTitleBar asks the same question when it reserves room for the
    // focus rule.
    a.setStyleSheet(dockStyleSheet(DockManager::testConfigFlag(DockManager::FocusHighlighting)));
#endif
    MainWindow w;
    w.show();
    return a.exec();
}
