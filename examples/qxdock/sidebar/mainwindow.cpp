#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "qxdock/dockwidget.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QPlainTextEdit>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Create the dock window. Because the parent parameter is a QMainWindow
    // the dock window registers itself as the central widget.
    QVBoxLayout *Layout = new QVBoxLayout(ui->dockContainer);
    Layout->setContentsMargins(QMargins(0, 0, 0, 0));
    m_window = new DockWindow(ui->dockContainer);
    Layout->addWidget(m_window);

    // Create example content label - this can be any application specific
    // widget
    QLabel *l = new QLabel();
    l->setWordWrap(true);
    l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    l->setText("good good study, day day up!");

    // Create a dock widget with the title Label 1 and set the created label
    // as the dock widget content
    DockWidget *dw = new DockWidget("Label 1");
    dw->setWidget(l);

    // Add the toggleViewAction of the dock widget to the menu to give
    // the user the possibility to show the dock widget if it has been closed
    ui->menuView->addAction(dw->toggleViewAction());

    // Add the dock widget to the top dock widget area
    m_window->addDockWidget(Qx::TopDockWidgetArea, dw);

    // Create an example editor
    QPlainTextEdit *te = new QPlainTextEdit();
    te->setPlaceholderText("Please enter your text here into this QPlainTextEdit...");
    dw = new DockWidget("Editor 1");
    dw->setWidget(te);
    ui->menuView->addAction(dw->toggleViewAction());
    m_window->addDockWidget(Qx::BottomDockWidgetArea, dw);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMainWindow::closeEvent(event);
    if (m_window) {
        m_window->deleteLater();
    }
}
