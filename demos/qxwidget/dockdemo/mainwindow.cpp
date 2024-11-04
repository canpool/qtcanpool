#include "mainwindow.h"

#include "qxdock/dockwindow.h"

QX_DOCK_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    DockWindow *dockwindow = new DockWindow();

    setCentralWidget(dockwindow);

    resize(800, 600);
}

MainWindow::~MainWindow()
{
}
