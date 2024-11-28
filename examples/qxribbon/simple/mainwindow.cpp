#include "mainwindow.h"

#include "qxribbon/ribbonbar.h"

QX_RIBBON_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : RibbonWindow(parent)
{
    RibbonBar *rb = ribbonBar();
    rb->addPage(tr("page1"));
    rb->addPage(tr("page2"));
}

MainWindow::~MainWindow()
{
}
