#include "mainwindow.h"

#include "ui_mainwindow.h"

#include <QWidgetAction>
#include <QFileSystemModel>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QSettings>
#include <QPlainTextEdit>
#include <QToolBar>

#include "qxdock/dockmanager.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockpanel.h"
#include "qxdock/dockautohidecontainer.h"

QX_DOCK_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    DockManager::setConfigFlag(DockManager::OpaqueSplitterResize, true);
    DockManager::setConfigFlag(DockManager::XmlCompressionEnabled, false);
    DockManager::setConfigFlag(DockManager::FocusHighlighting, true);
    DockManager::setAutoHideConfigFlags(DockManager::DefaultAutoHideConfig);

    m_window = new DockWindow(this);
    setCentralWidget(m_window);

    // Set central widget
    QPlainTextEdit *w = new QPlainTextEdit();
    w->setPlaceholderText("This is the central editor. Enter your text here.");
    DockWidget *centralDockWidget = new DockWidget("CentralWidget");
    centralDockWidget->setWidget(w);
    auto *centralPanel = m_window->setCentralWidget(centralDockWidget);
    centralPanel->setAllowedAreas(Qx::OuterDockAreas);

    // create other dock widgets
    QTableWidget *table = new QTableWidget();
    table->setColumnCount(3);
    table->setRowCount(10);
    DockWidget *tableDockWidget = new DockWidget("Table 1");
    tableDockWidget->setWidget(table);
    tableDockWidget->setMinimumSizeHintMode(DockWidget::MinimumSizeHintFromDockWidget);
    tableDockWidget->setMinimumSize(200, 150);
    const auto autoHideContainer = m_window->addAutoHideDockWidget(Qx::DockSideBarLeft, tableDockWidget);
    autoHideContainer->setSize(480);
    ui->menuView->addAction(tableDockWidget->toggleViewAction());

    table = new QTableWidget();
    table->setColumnCount(5);
    table->setRowCount(1020);
    tableDockWidget = new DockWidget("Table 2");
    tableDockWidget->setWidget(table);
    tableDockWidget->setMinimumSizeHintMode(DockWidget::MinimumSizeHintFromDockWidget);
    tableDockWidget->resize(250, 150);
    tableDockWidget->setMinimumSize(200, 150);
    m_window->addAutoHideDockWidget(Qx::DockSideBarLeft, tableDockWidget);
    ui->menuView->addAction(tableDockWidget->toggleViewAction());

    QTableWidget *propertiesTable = new QTableWidget();
    propertiesTable->setColumnCount(3);
    propertiesTable->setRowCount(10);
    DockWidget *propertiesDockWidget = new DockWidget("Properties");
    propertiesDockWidget->setWidget(propertiesTable);
    propertiesDockWidget->setMinimumSizeHintMode(DockWidget::MinimumSizeHintFromDockWidget);
    propertiesDockWidget->resize(250, 150);
    propertiesDockWidget->setMinimumSize(200, 150);
    m_window->addDockWidget(Qx::RightDockWidgetArea, propertiesDockWidget, centralPanel);
    ui->menuView->addAction(propertiesDockWidget->toggleViewAction());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Delete dock window here to delete all floating widgets. This ensures
    // that all top level widgets of the dock window are properly closed
    m_window->deleteLater();
    QMainWindow::closeEvent(event);
}
