#include "mainwindow.h"

#include "qxdock/dockwindow.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockpanel.h"
#include "qxdock/dockmanager.h"

#include <QMenuBar>
#include <QMenu>
#include <QLabel>
#include <QTableWidget>

QX_DOCK_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QIcon icon(":/logo");

    QMenuBar *mb = menuBar();
    QMenu *menuView = mb->addMenu(tr("View"));

    DockManager::setConfigFlag(DockManager::OpaqueSplitterResize, true);
    DockManager::setConfigFlag(DockManager::XmlCompressionEnabled, false);
    DockManager::setConfigFlag(DockManager::FocusHighlighting, true);

    DockWindow *wd = new DockWindow(this);
    setCentralWidget(wd);

    // Set central widget
    QLabel *label = new QLabel();
    label->setText("This is a DockArea which is always visible, even if it does not contain any DockWidgets.");
    label->setAlignment(Qt::AlignCenter);
    DockWidget *centralDockWidget = new DockWidget("CentralWidget");
    centralDockWidget->setWidget(label);
    centralDockWidget->setFeature(DockWidget::NoTab, true);
    auto *centralPanel = wd->setCentralWidget(centralDockWidget);

    // create other dock widgets
    QTableWidget *table = new QTableWidget();
    table->setColumnCount(3);
    table->setRowCount(10);
    DockWidget *tableDockWidget = new DockWidget("Table 1");
    tableDockWidget->setWidget(table);
    tableDockWidget->setMinimumSizeHintMode(DockWidget::MinimumSizeHintFromDockWidget);
    tableDockWidget->resize(250, 150);
    tableDockWidget->setMinimumSize(200, 150);
    auto tabPanel = wd->addDockWidget(Qx::LeftDockWidgetArea, tableDockWidget);
    menuView->addAction(tableDockWidget->toggleViewAction());

    table = new QTableWidget();
    table->setColumnCount(5);
    table->setRowCount(1020);
    tableDockWidget = new DockWidget("Table 2");
    tableDockWidget->setWidget(table);
    tableDockWidget->setMinimumSizeHintMode(DockWidget::MinimumSizeHintFromDockWidget);
    tableDockWidget->resize(250, 150);
    tableDockWidget->setMinimumSize(200, 150);
    wd->addDockWidget(Qx::BottomDockWidgetArea, tableDockWidget, tabPanel);
    menuView->addAction(tableDockWidget->toggleViewAction());

    QTableWidget *propertiesTable = new QTableWidget();
    propertiesTable->setColumnCount(3);
    propertiesTable->setRowCount(10);
    DockWidget *propertiesDockWidget = new DockWidget("Properties");
    propertiesDockWidget->setWidget(propertiesTable);
    propertiesDockWidget->setMinimumSizeHintMode(DockWidget::MinimumSizeHintFromDockWidget);
    propertiesDockWidget->resize(250, 150);
    propertiesDockWidget->setMinimumSize(200, 150);
    wd->addDockWidget(Qx::RightDockWidgetArea, propertiesDockWidget, centralPanel);
    menuView->addAction(propertiesDockWidget->toggleViewAction());

    setWindowTitle(tr("Dock Empty DockArea Example"));
    setWindowIcon(icon);
}

MainWindow::~MainWindow()
{
}
