#include "mainwindow.h"
#include "qlite/litebar.h"

MainWindow::MainWindow(QWidget *parent)
    : QLite::LiteWindow(parent)
{
    liteBar()->setIconDark(true);
    setWindowIcon(QIcon(":/resource/image/logo.png"));
    setWindowTitle(tr("template"));
}

MainWindow::~MainWindow()
{

}
