#include "mainwindow.h"

#include "qxdock/dockwindow.h"
#include "qxdock/dockwidget.h"

#include <QMenuBar>
#include <QMenu>
#include <QLabel>
#include <QAction>
#include <QStackedLayout>
#include <QPushButton>

QX_DOCK_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QIcon icon(":/logo");

    QMenuBar *mb = menuBar();
    QMenu *menuFile = mb->addMenu(tr("File"));
    m_actionOpen = menuFile->addAction(tr("Open project"));
    m_actionClose = menuFile->addAction(tr("Close project"));
    m_menuView = mb->addMenu(tr("View"));

    QWidget *centralWidget = new QWidget();
    setCentralWidget(centralWidget);

    centralWidget->setLayout(m_layout = new QStackedLayout());

    m_welcomeWidget = new QWidget(this);
    auto welcomeLayout = new QVBoxLayout(m_welcomeWidget);
    welcomeLayout->addStretch();
    QPushButton *openButton = new QPushButton("Open project");
    welcomeLayout->addWidget(openButton);
    welcomeLayout->addStretch();

    connect(openButton, SIGNAL(clicked()), this, SLOT(openProject()));

    m_window = new DockWindow(centralWidget);

    // Create example content label - this can be any application specific
    // widget
    QLabel *l = new QLabel();
    l->setWordWrap(true);
    l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    l->setText("good good study, day day up!");

    // Create a dock widget with the title Label 1 and set the created label
    // as the dock widget content
    DockWidget *dockWidget = new DockWidget("Label 1");
    dockWidget->setWidget(l);

    // Add the toggleViewAction of the dock widget to the menu to give
    // the user the possibility to show the dock widget if it has been closed
    m_menuView->addAction(dockWidget->toggleViewAction());

    connect(m_actionOpen, SIGNAL(triggered()), this, SLOT(openProject()));
    connect(m_actionClose, SIGNAL(triggered()), this, SLOT(closeProject()));

    // Add the dock widget to the top dock widget area
    m_window->addDockWidget(Qx::TopDockWidgetArea, dockWidget);

    centralWidget->layout()->addWidget(m_welcomeWidget);
    centralWidget->layout()->addWidget(m_window);

    closeProject();

    setWindowTitle(tr("Dock HideShow Example"));
    setWindowIcon(icon);
}

MainWindow::~MainWindow()
{
}

void MainWindow::openProject()
{
    m_actionOpen->setEnabled(false);
    m_actionClose->setEnabled(true);
    m_menuView->setEnabled(true);

    m_layout->setCurrentWidget(m_window);
}

void MainWindow::closeProject()
{
    m_actionOpen->setEnabled(true);
    m_actionClose->setEnabled(false);
    m_menuView->setEnabled(false);

    m_window->hideWindowAndFloatingWidgets();
    m_layout->setCurrentWidget(m_welcomeWidget);
}
