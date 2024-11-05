#include "mainwindow.h"

#include "qxdock/dockwindow.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockpanel.h"

QX_DOCK_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    DockWindow *dockwindow = new DockWindow();

    DockWidget *w = nullptr;

    w = new DockWidget();
    QPalette palette = w->palette();
    palette.setColor(QPalette::Window, QColor(255, 0, 0));
    w->setPalette(palette);
    w->setAutoFillBackground(true);

    DockPanel *panel = dockwindow->addDockWidget(Qx::LeftDockWidgetArea, w);

    w = new DockWidget();
    palette = w->palette();
    palette.setColor(QPalette::Window, QColor(0, 255, 0));
    w->setPalette(palette);
    w->setAutoFillBackground(true);

    dockwindow->addDockWidget(Qx::RightDockWidgetArea, w, panel);

    setCentralWidget(dockwindow);

    resize(800, 600);
}

MainWindow::~MainWindow()
{
}
