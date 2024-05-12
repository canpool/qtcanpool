#include <QApplication>
#include <QDesktopWidget>
#include "mainwindow.h"


/* MainWindow */
MainWindow::MainWindow(QWidget* parent)
    : Qtitan::RibbonMainWindow(parent)
{
    ui.setupUi(this);

    QRect geom = QApplication::desktop()->availableGeometry();
    resize(2 * geom.width() / 3, 2 * geom.height() / 3);
}

MainWindow::~MainWindow()
{
}


