#include "mainwindow.h"

#include "qxdock/dockwindow.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockpanel.h"
#include "qxdock/dockmanager.h"

#include <QPlainTextEdit>
#include <QTableWidget>
#include <QMenuBar>
#include <QMenu>
#include <QInputDialog>
#include <QSignalBlocker>

QX_DOCK_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QIcon icon(":/logo");

    QMenuBar *mb = menuBar();
    QMenu *menuView = mb->addMenu(tr("View"));
    m_toolBar = addToolBar("toolbar");

    DockManager::setConfigFlag(DockManager::OpaqueSplitterResize, true);
    DockManager::setConfigFlag(DockManager::XmlCompressionEnabled, false);
    DockManager::setConfigFlag(DockManager::FocusHighlighting, true);

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
    tableDockWidget->resize(250, 150);
    tableDockWidget->setMinimumSize(200, 150);
    auto tablePanel = m_window->addDockWidget(Qx::LeftDockWidgetArea, tableDockWidget);
    menuView->addAction(tableDockWidget->toggleViewAction());

    table = new QTableWidget();
    table->setColumnCount(5);
    table->setRowCount(1020);
    tableDockWidget = new DockWidget("Table 2");
    tableDockWidget->setWidget(table);
    tableDockWidget->setMinimumSizeHintMode(DockWidget::MinimumSizeHintFromDockWidget);
    tableDockWidget->resize(250, 150);
    tableDockWidget->setMinimumSize(200, 150);
    m_window->addDockWidget(Qx::BottomDockWidgetArea, tableDockWidget, tablePanel);
    menuView->addAction(tableDockWidget->toggleViewAction());

    QTableWidget *propertiesTable = new QTableWidget();
    propertiesTable->setColumnCount(3);
    propertiesTable->setRowCount(10);
    DockWidget *propertiesDockWidget = new DockWidget("Properties");
    propertiesDockWidget->setWidget(propertiesTable);
    propertiesDockWidget->setMinimumSizeHintMode(DockWidget::MinimumSizeHintFromDockWidget);
    propertiesDockWidget->resize(250, 150);
    propertiesDockWidget->setMinimumSize(200, 150);
    m_window->addDockWidget(Qx::RightDockWidgetArea, propertiesDockWidget, centralPanel);
    menuView->addAction(propertiesDockWidget->toggleViewAction());

    createPerspectiveUi();

    setWindowTitle(tr("Dock CentralWidget Example"));
    setWindowIcon(icon);
}

MainWindow::~MainWindow()
{
}

/*
default state contents:
-----------------------

<?xml version=\"1.0\" encoding=\"UTF-8\"?>
<QxDock Version=\"1\" UserVersion=\"0\" Containers=\"1\" CentralWidget=\"CentralWidget\">
	<Container Floating=\"0\">
		<Splitter Orientation=\"|\" Count=\"3\">
			<Splitter Orientation=\"-\" Count=\"2\">
				<Area Tabs=\"1\" Current=\"Table 1\">
					<Widget Name=\"Table 1\" Closed=\"0\"/>
				</Area>
				<Area Tabs=\"1\" Current=\"Table 2\">
					<Widget Name=\"Table 2\" Closed=\"0\"/>
				</Area>
			</Splitter>
			<Sizes>211 210 </Sizes>
			<Area Tabs=\"1\" Current=\"CentralWidget\" AllowedAreas=\"f\" Flags=\"1\">
				<Widget Name=\"CentralWidget\" Closed=\"0\"/>
			</Area>
			<Area Tabs=\"1\" Current=\"Properties\">
				<Widget Name=\"Properties\" Closed=\"0\"/>
			</Area>
			<Sizes>256 246 256 </Sizes>
		</Splitter>
	</Container>
</QxDock>\n"

-----------------------
*/

void MainWindow::savePerspective()
{
    QString perspectiveName = QInputDialog::getText(this, "Save Perspective", "Enter unique name:");
    if (perspectiveName.isEmpty()) {
        return;
    }

    m_window->addPerspective(perspectiveName);
    QSignalBlocker blocker(m_perspectiveComboBox);
    m_perspectiveComboBox->clear();
    m_perspectiveComboBox->addItems(m_window->perspectiveNames());
    m_perspectiveComboBox->setCurrentText(perspectiveName);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Delete dock window here to delete all floating widgets. This ensures
    // that all top level widgets of the dock window are properly closed
    m_window->deleteLater();
    QMainWindow::closeEvent(event);
}

void MainWindow::createPerspectiveUi()
{
    m_savePerspectiveAction = new QAction("Create Perspective", this);
    connect(m_savePerspectiveAction, SIGNAL(triggered()), SLOT(savePerspective()));
    m_perspectiveListAction = new QWidgetAction(this);
    m_perspectiveComboBox = new QComboBox(this);
    m_perspectiveComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_perspectiveComboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    connect(m_perspectiveComboBox, SIGNAL(currentTextChanged(const QString &)), m_window,
            SLOT(openPerspective(const QString &)));
    m_perspectiveListAction->setDefaultWidget(m_perspectiveComboBox);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_perspectiveListAction);
    m_toolBar->addAction(m_savePerspectiveAction);
}
