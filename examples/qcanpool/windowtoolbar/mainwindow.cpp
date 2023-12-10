#include "mainwindow.h"

#include <QAction>

#include "qcanpool/windowtoolbar.h"

QCANPOOL_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QIcon icon(":/logo");
    WindowToolBar *wtb = new WindowToolBar(this);

    setMenuWidget(wtb);

    wtb->addAction(icon, tr("action1"));
    wtb->addSeparator();
    QAction *act = wtb->addAction(icon, tr("action2"));
    wtb->insertAction(act, new QAction(icon, tr("action3")));

    setWindowTitle(tr("WindowToolBar Example"));
    setWindowIcon(icon);
    resize(400, 200);
}

MainWindow::~MainWindow()
{
}

