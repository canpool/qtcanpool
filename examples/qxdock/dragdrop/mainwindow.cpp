/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "mainwindow.h"

#include "qxdock/dockwindow.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockdrag.h"

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QTextEdit>
#include <QStatusBar>

QX_DOCK_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QMenu *file = menuBar()->addMenu(tr("File"));
    QAction *drag = file->addAction(tr("Drag Notes out (native)"));
    connect(drag, &QAction::triggered, this, &MainWindow::startNativeDrag);

    m_window = new DockWindow(this);
    setCentralWidget(m_window);

    QTextEdit *editor = new QTextEdit();
    editor->setPlainText(tr("This window accepts native dock-widget drops (C3). "
                            "Close Notes, then choose File > Drag Notes out (native) "
                            "and drop it back here: the window restores its own "
                            "closed widget and reports the drop through the "
                            "dockWidgetDropped() signal. Drop a dock widget dragged "
                            "from another qxdock-based application to exercise the "
                            "cross-process path."));
    DockWidget *central = new DockWidget("Editor");
    central->setWidget(editor);
    m_window->setCentralWidget(central);

    // A closable, non-deleting dock widget. It keeps its object alive when closed
    // (no DeleteOnClose) and carries an object name + title the receiver can read
    // from the drop payload (QxDock/1\n<name>\n<title>).
    m_notes = new DockWidget("Notes");
    m_notes->setObjectName(QStringLiteral("Notes"));
    m_notes->setWidget(new QTextEdit(tr("Notes content")));
    m_window->addDockWidget(Qx::RightDockWidgetArea, m_notes);

    connect(m_window, &DockWindow::dockWidgetDropped, this, &MainWindow::onDockWidgetDropped);

    resize(960, 600);
}

MainWindow::~MainWindow()
{
}

void MainWindow::startNativeDrag()
{
    // Close it first so the window treats the drop as "bring back a closed
    // widget of its own". startDrag() blocks like QDrag::exec().
    if (!m_notes->isClosed())
        m_notes->toggleView(false);
    DockDrag::startDrag(this, m_notes, Qt::MoveAction);
}

void MainWindow::onDockWidgetDropped(const QString &name, const QPoint &globalPos)
{
    statusBar()->showMessage(
        tr("Dropped dock widget '%1' at (%2, %3)").arg(name).arg(globalPos.x()).arg(globalPos.y()));
}
