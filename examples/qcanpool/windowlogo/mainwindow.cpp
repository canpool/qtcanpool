#include "mainwindow.h"

#include <QMenu>
#include <QToolBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>

#include "qcanpool/windowlogo.h"

QCANPOOL_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QPixmap pixmap(":/logo");
    QIcon icon(pixmap);

    QToolBar *tb = addToolBar(tr("tb"));
    WindowLogo *logo = new WindowLogo(pixmap, this);
    logo->setRound(true);
    connect(logo, &WindowLogo::clicked, this, [&](){
        qDebug() << "logo clicked";
    });
    tb->addWidget(logo);
    tb->addAction(icon, tr("action1"));

    QWidget *w = new QWidget(this);
    QHBoxLayout *hl = new QHBoxLayout();
    logo = new WindowLogo(pixmap, this);
    logo->setRound(true);
    hl->addWidget(logo);
    logo = new WindowLogo(pixmap, this);
    hl->addWidget(logo);
    logo = new WindowLogo(pixmap, this);
    hl->addWidget(logo);
    hl->addStretch();
    QVBoxLayout *vl = new QVBoxLayout();
    vl->addLayout(hl);
    vl->addStretch();
    w->setLayout(vl);
    setCentralWidget(w);

    setWindowTitle(tr("WindowLogo Example"));
    setWindowIcon(icon);
    resize(400, 200);
}

MainWindow::~MainWindow()
{
}

