#include "mainwindow.h"
#include "stateprogressbar.h"

#include <QVBoxLayout>
#include <QDesktopWidget>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createWindow();

    QRect geom = QApplication::desktop()->availableGeometry();
    resize(2 * geom.width() / 3, 2 * geom.height() / 3);
    raise();
    activateWindow();
}

MainWindow::~MainWindow()
{

}

void MainWindow::createWindow()
{
    createStateProgressBar();

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    layout->addWidget(m_pStateProgressBar);
    layout->addStretch();

    QWidget *w = new QWidget();
    w->setLayout(layout);
    setCentralWidget(w);
}

void MainWindow::createStateProgressBar()
{
    m_pStateProgressBar = new StateProgressBar();
    m_pStateProgressBar->setType(StateProgressBar::ACK); // NACK, ACK
    QStringList states;
    states<<tr("C")<<tr("C++")<<tr("Linux")<<tr("Qt")<<tr("Java");
    m_pStateProgressBar->setStates(states);
    m_pStateProgressBar->setTip("now");
    m_pStateProgressBar->setValue(4);
    m_pStateProgressBar->setFixedHeight(140);
}
