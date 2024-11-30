#include "mainwindow.h"

#include "qxribbon/ribbonutils.h"

#include <QToolBar>
#include <QAction>
#include <QColor>

QX_RIBBON_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QIcon icon(":/logo");

    QToolBar *tb = addToolBar(tr("tb"));
    tb->addAction(icon, tr("action1"));
    tb->addAction(QxRibbon::colorIcon(QColor(), tb->iconSize()), tr("action2"));
    tb->addAction(colorIcon(QColor(255, 128, 128), tb->iconSize()), tr("action3"));
    tb->addAction(colorIcon(Qt::green, tb->iconSize()), tr("action4"));

    resize(400, 200);
}

MainWindow::~MainWindow()
{
}

