/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "mainwindow.h"

#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbongroup.h"
#include "qxribbon/ribbonpage.h"
#include "qxribbon/ribbonsliderpane.h"
#include "qxribbon/ribbonstatusbar.h"

#include <QAction>
#include <QIcon>
#include <QTextEdit>

QX_RIBBON_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : RibbonMainWindow(parent)
{
    auto *editor = new QTextEdit();
    editor->setPlainText(tr("The bar at the bottom is a RibbonStatusBar.\n\n"
                            "Left: plain-text segments the host keeps up to date, separated by hairlines.\n"
                            "Right: view switches that stay put.\n\n"
                            "Press Show message: the text scrolls across the left and the segments step aside "
                            "until it clears - the switches on the right never move. Press Advance: the "
                            "second segment moves on, because its action's text is what the segment shows."));
    setCentralWidget(editor);

    RibbonBar *rb = ribbonBar();
    RibbonPage *page = rb->addPage(tr("Home"));
    RibbonGroup *group = page->addGroup(tr("Status"));
    QAction *show = new QAction(QIcon(":/logo"), tr("Show message"), this);
    QAction *advance = new QAction(QIcon(":/logo"), tr("Advance"), this);
    group->addLargeAction(show);
    group->addLargeAction(advance);

    auto *bar = new RibbonStatusBar();
    setStatusBar(bar);

    // Left, temporary section: status text the host keeps up to date. Setting the
    // action's text is all it takes - the segment follows it.
    QAction *document = bar->addAction(tr("Ready"));
    bar->addSeparator();
    QAction *position = bar->addAction(tr("Ln 1, Col 1"));

    // Right, permanent section: the switches, which never yield to a message.
    QAction *normal = new QAction(QIcon(":/logo"), tr("Normal"), this);
    QAction *pageLayout = new QAction(QIcon(":/logo"), tr("Page Layout"), this);
    bar->addPermanentAction(normal);
    bar->addPermanentAction(pageLayout);

    // The zoom control is a plain widget, so it joins the permanent section
    // through the status bar's own addPermanentWidget() - the pane is a host
    // widget like any other, the bar only carries it.
    RibbonSliderPane *zoom = new RibbonSliderPane();
    zoom->setRange(10, 500);
    zoom->setValue(100);
    bar->addPermanentWidget(zoom);

    connect(show, &QAction::triggered, bar, [bar] {
        bar->showMessage(tr("A message takes the left section for a moment"), 2500);
    });
    connect(advance, &QAction::triggered, this, [this, position] {
        ++m_line;
        position->setText(tr("Ln %1, Col 1").arg(m_line));
    });
    connect(normal, &QAction::triggered, this, [document] {
        document->setText(tr("Normal view"));
    });
    connect(pageLayout, &QAction::triggered, this, [document] {
        document->setText(tr("Page Layout view"));
    });
    connect(zoom, &RibbonSliderPane::valueChanged, this, [document](int value) {
        document->setText(tr("Zoom %1%").arg(value));
    });

    resize(960, 600);
}

MainWindow::~MainWindow()
{
}
