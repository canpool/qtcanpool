#include "mainwindow.h"

#include "qxdock/dockcontainer.h"

QX_DOCK_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    DockContainer *container = new DockContainer();

    setCentralWidget(container);
}

MainWindow::~MainWindow()
{
}
