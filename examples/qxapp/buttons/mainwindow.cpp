/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "mainwindow.h"

#include "qxapp/qxtoolbutton.h"
#include "qxapp/qxmenubutton.h"
#include "qxapp/qxmenuaccessbutton.h"
#include "qxapp/qxextensionbutton.h"

#include <QIcon>
#include <QMenu>
#include <QVBoxLayout>

QX_APP_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(640, 360);
    setWindowTitle(tr("QxToolButton / QxMenuButton / QxMenuAccessButton / QxExtensionButton"));

    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);

    // QxToolButton: the menu arrow lands below the label only under the
    // TextUnderIcon style - any other style forces it back to the right side.
    auto *tool = new QxToolButton(QIcon(), tr("Tool"), this);
    tool->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    tool->setMenuArea(QxToolButton::BottomMenuArea);
    auto *toolMenu = new QMenu(this);
    toolMenu->addAction(tr("Action one"));
    toolMenu->addAction(tr("Action two"));
    tool->setMenu(toolMenu);
    layout->addWidget(tool);

    // QxMenuButton: a plain tool button that always paints a menu arrow.
    auto *menu = new QxMenuButton(QIcon(), tr("Menu"), this);
    auto *menuMenu = new QMenu(this);
    menuMenu->addAction(tr("Open"));
    menuMenu->addAction(tr("Save"));
    menu->setMenu(menuMenu);
    layout->addWidget(menu);

    // QxMenuAccessButton: the access arrow flips with the orientation.
    auto *access = new QxMenuAccessButton(this);
    access->setText(tr("Access"));
    access->setOrientation(Qt::Vertical);
    auto *accessMenu = new QMenu(this);
    accessMenu->addAction(tr("Configure"));
    access->setMenu(accessMenu);
    layout->addWidget(access);

    // QxExtensionButton: a collapsible marker that paints a triangle.
    auto *ext = new QxExtensionButton(this);
    ext->setText(tr("More"));
    ext->setOrientation(Qt::Horizontal);
    layout->addWidget(ext);

    layout->addStretch(1);
    setCentralWidget(central);
}

MainWindow::~MainWindow() = default;
