// Copyright (C) 2026 maminjie <canpool@163.com>
// SPDX-License-Identifier: MulanPSL-2.0

#include "mainwindow.h"

#include "qxribbon/ribbonpage.h"
#include "qxribbon/ribbongroup.h"

#include <QAction>
#include <QStyle>

QX_APP_USE_NAMESPACE
QX_RIBBON_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QxAppShell(parent)
{
    // One call builds the standard shell and returns the page it lives on.
    // The page already carries the Layout group from addLayoutGroup().
    RibbonPage *home = createStandardShell();

    // This application's own groups follow the standard one.
    RibbonGroup *edit = home->addGroup(tr("Edit"));
    QAction *cut = new QAction(style()->standardIcon(QStyle::SP_DialogCloseButton), tr("Cut"), this);
    QAction *copy = new QAction(style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Copy"), this);
    edit->addLargeAction(cut);
    edit->addLargeAction(copy);

    RibbonGroup *view = home->addGroup(tr("View"));
    QAction *zoomIn = new QAction(tr("Zoom In"), this);
    QAction *zoomOut = new QAction(tr("Zoom Out"), this);
    view->addAction(zoomIn);
    view->addAction(zoomOut);

    resize(800, 500);
}
