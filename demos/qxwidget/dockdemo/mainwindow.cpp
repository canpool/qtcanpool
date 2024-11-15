#include "mainwindow.h"

#include "qxdock/dockwindow.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockpanel.h"
#include "qxdock/dockmanager.h"

#include <QMenuBar>
#include <QMenu>
#include <QTextEdit>

QX_DOCK_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    QMenuBar *mb = menuBar();
    QMenu *viewMenu = mb->addMenu(tr("View"));

    // set auto hide config before new DockWindow
    DockManager::setAutoHideConfigFlags(DockManager::DefaultAutoHideConfig);
    DockManager::setConfigFlag(DockManager::FocusHighlighting, true);

    DockWindow *dockwindow = new DockWindow();

    QTextEdit *te = nullptr;
    QWidget *cw = nullptr;

    DockWidget *w = nullptr;

    te = new QTextEdit(tr("AutoScrollArea"));
    w = createColorDockWidget(tr("dock1"), QColor(255, 0, 0));
    w->setIcon(QIcon(":/res/tab.svg"));
    w->setWidget(te);
    viewMenu->addAction(w->toggleViewAction());
    DockPanel *panel = dockwindow->addDockWidget(Qx::LeftDockWidgetArea, w);

    w = createColorDockWidget(tr("dock2"), QColor(0, 255, 0));
    viewMenu->addAction(w->toggleViewAction());
    dockwindow->addDockWidget(Qx::RightDockWidgetArea, w, panel);

    cw = createColorWidget(Qt::lightGray);
    w = createColorDockWidget(tr("looooooooooooooooongdock"), QColor(128, 128, 128));
    w->setWidget(cw, DockWidget::ForceScrollArea);
    viewMenu->addAction(w->toggleViewAction());
    dockwindow->addDockWidget(Qx::RightDockWidgetArea, w, panel);

    w = createColorDockWidget(tr("dock4"), QColor(255, 255, 0));
    viewMenu->addAction(w->toggleViewAction());
    dockwindow->addDockWidget(Qx::CenterDockWidgetArea, w, panel);  // tabified with dock1

    w = createColorDockWidget(tr("dock5"), QColor(0, 255, 255));
    viewMenu->addAction(w->toggleViewAction());
    // tabified with dock1, because of dock1 panel is Left area panel
    dockwindow->addDockWidgetTab(Qx::LeftDockWidgetArea, w);

    w = createColorDockWidget(tr("dock3"), QColor(0, 0, 255));
    viewMenu->addAction(w->toggleViewAction());
    panel = dockwindow->addDockWidget(Qx::BottomDockWidgetArea, w);

    cw = createColorWidget(Qt::darkCyan);
    w = createColorDockWidget(tr("dock6"), QColor(255, 0, 255));
    w->setWidget(cw, DockWidget::ForceNoScrollArea);
    viewMenu->addAction(w->toggleViewAction());
    dockwindow->addDockWidgetTab(w, panel); // tabified with dock3

    cw = createColorWidget(Qt::darkRed);
    w = createColorDockWidget(tr("auto1"), QColor(255, 100, 100));
    w->setWidget(cw);
    viewMenu->addAction(w->toggleViewAction());
    dockwindow->addAutoHideDockWidget(Qx::DockSideBarLeft, w);

    setCentralWidget(dockwindow);

    resize(800, 600);
}

MainWindow::~MainWindow()
{
}

DockWidget *MainWindow::createColorDockWidget(const QString &title, const QColor &color)
{
    DockWidget *w = new DockWidget(title);
    QPalette palette = w->palette();
    palette.setColor(QPalette::Window, color);
    w->setPalette(palette);
    w->setAutoFillBackground(true);
    return w;
}

QWidget *MainWindow::createColorWidget(const QColor &color)
{
    QWidget *w = new QWidget();
    QPalette palette = w->palette();
    palette.setColor(QPalette::Window, color);
    w->setPalette(palette);
    w->setAutoFillBackground(true);
    return w;
}
