#include "mainwindow.h"

#include "qcanpool/fancybar.h"

#include <QToolBar>

MainWindow::MainWindow(QWidget *parent)
    : FancyWindow(parent)
{
    FancyBar *fb = fancyBar();
    QToolBar *tb = fb->toolBar();
    tb->addAction(tr("action1"), this, [this]() {
        Dialog dlg;
        dlg.exec();
    });
    // toolBar in menuWidget is hidden by default
    fb->showMenuBar(true);
}

MainWindow::~MainWindow()
{
}

Dialog::Dialog(QWidget *parent, Qt::WindowFlags f)
    : FancyDialog(parent, f)
{

}

Dialog::~Dialog()
{

}
