#include "mainwindow.h"

#include "qxdock/dockwindow.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockmanager.h"

#include <QMenuBar>
#include <QMenu>
#include <QLabel>
#include <QToolBar>

QX_DOCK_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QIcon icon(":/logo");

    QMenuBar *mb = menuBar();
    QMenu *menuView = mb->addMenu(tr("View"));

    // Add the toolbar
    QToolBar *tb = addToolBar("Top Toolbar");

    // Create the dock manager
    DockManager::setConfigFlags(DockManager::DefaultOpaqueConfig);
    DockManager::setConfigFlag(DockManager::DockAreaHasCloseButton, false);
    DockManager::setConfigFlag(DockManager::DockAreaHasUndockButton, false);
    DockManager::setConfigFlag(DockManager::DockAreaHasTabsMenuButton, false);

    DockWindow *wd = new DockWindow(this);
    setCentralWidget(wd);

    // Create a dockable widget
    QLabel *l1 = new QLabel();
    l1->setWordWrap(true);
    l1->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    l1->setText("Docking widget 1");
    DockWidget *dockWidget1 = new DockWidget("Dock 1");
    dockWidget1->setWidget(l1);
    wd->addDockWidget(Qx::LeftDockWidgetArea, dockWidget1);

    QLabel *l2 = new QLabel();
    l2->setWordWrap(true);
    l2->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    l2->setText("Docking widget 2");
    DockWidget *dockWidget2 = new DockWidget("Dock 2");
    dockWidget2->setWidget(l2);
    wd->addDockWidget(Qx::RightDockWidgetArea, dockWidget2);

    // Add menu actions
    menuView->addAction(dockWidget1->toggleViewAction());
    menuView->addAction(dockWidget2->toggleViewAction());
    tb->addAction(dockWidget1->toggleViewAction());
    tb->addAction(dockWidget2->toggleViewAction());

    setWindowTitle(tr("Dock ConfigFlags Example"));
    setWindowIcon(icon);
}

MainWindow::~MainWindow()
{
}
