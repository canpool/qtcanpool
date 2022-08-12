#include "mainwindow.h"
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QLabel *label = new QLabel("I CAN DO IT");
    label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    setCentralWidget(label);
}

MainWindow::~MainWindow()
{
}

