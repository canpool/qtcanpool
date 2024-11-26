#include "mainwindow.h"

#include "qxdock/dockwindow.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockpanel.h"
#include "qxdock/dockmanager.h"

#include <QMenuBar>
#include <QMenu>
#include <QTextEdit>
#include <QSettings>

QX_DOCK_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QMenuBar *mb = menuBar();
    QMenu *viewMenu = mb->addMenu(tr("View"));

    DockManager::setConfigFlag(DockManager::FocusHighlighting, true);
    // set auto hide config before new DockWindow
    DockManager::setAutoHideConfigFlags(DockManager::DefaultAutoHideConfig);

    m_dockWindow = new DockWindow();
    DockWindow *dockwindow = m_dockWindow;

    QTextEdit *te = nullptr;
    QWidget *cw = nullptr;

    DockWidget *w = nullptr;

    te = new QTextEdit(tr("AutoScrollArea"));
    w = new DockWidget(tr("dock1"));
    w->setIcon(QIcon(":/res/tab.svg"));
    w->setWidget(te);
    viewMenu->addAction(w->toggleViewAction());
    DockPanel *panel = dockwindow->addDockWidget(Qx::LeftDockWidgetArea, w);

    w = new DockWidget(tr("dock2"));
    viewMenu->addAction(w->toggleViewAction());
    dockwindow->addDockWidget(Qx::RightDockWidgetArea, w, panel);

    cw = new QWidget();
    w = new DockWidget(tr("looooooooooooooooongdock"));
    w->setWidget(cw, DockWidget::ForceScrollArea);
    viewMenu->addAction(w->toggleViewAction());
    dockwindow->addDockWidget(Qx::RightDockWidgetArea, w, panel);

    w = new DockWidget(tr("dock4"));
    viewMenu->addAction(w->toggleViewAction());
    dockwindow->addDockWidget(Qx::CenterDockWidgetArea, w, panel);   // tabified with dock1

    w = new DockWidget(tr("dock5"));
    viewMenu->addAction(w->toggleViewAction());
    // tabified with dock1, because of dock1 panel is Left area panel
    dockwindow->addDockWidgetTab(Qx::LeftDockWidgetArea, w);

    w = new DockWidget(tr("dock3"));
    viewMenu->addAction(w->toggleViewAction());
    panel = dockwindow->addDockWidget(Qx::BottomDockWidgetArea, w);

    cw = new QWidget();
    w = new DockWidget(tr("dock6"));
    w->setWidget(cw, DockWidget::ForceNoScrollArea);
    viewMenu->addAction(w->toggleViewAction());
    dockwindow->addDockWidgetTab(w, panel);   // tabified with dock3

    cw = new QWidget();
    w = new DockWidget(tr("auto1"));
    w->setWidget(cw);
    viewMenu->addAction(w->toggleViewAction());
    dockwindow->addAutoHideDockWidget(Qx::DockSideBarLeft, w);

    // edit menu
    QMenu *editMenu = mb->addMenu(tr("Edit"));
    editMenu->addAction(tr("saveState"), [=]() {
        this->saveState();
    });
    editMenu->addAction(tr("restoreState"), [=]() {
        this->restoreState();
    });

    setCentralWidget(dockwindow);

    resize(800, 600);

    restoreState();
}

MainWindow::~MainWindow()
{
}

void MainWindow::saveState()
{
    QSettings settings("Settings.ini", QSettings::IniFormat);
    settings.setValue("mainWindow/Geometry", this->saveGeometry());
    settings.setValue("mainWindow/State", QMainWindow::saveState());
    settings.setValue("mainWindow/DockingState", m_dockWindow->saveState());
}

void MainWindow::restoreState()
{
    QSettings settings("Settings.ini", QSettings::IniFormat);
    this->restoreGeometry(settings.value("mainWindow/Geometry").toByteArray());
    QMainWindow::restoreState(settings.value("mainWindow/State").toByteArray());
    m_dockWindow->restoreState(settings.value("mainWindow/DockingState").toByteArray());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveState();
    QMainWindow::closeEvent(event);
}
