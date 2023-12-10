#include "mainwindow.h"

#include <QToolBar>
#include <QMenu>

#include "qcanpool/extensionbutton.h"

QCANPOOL_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QIcon icon(":/logo");

    QToolBar *tb = addToolBar(tr("tb"));
    tb->addAction(icon, tr("act1"));

    QMenu *menu = new QMenu(this);
    menu->addAction(icon, tr("action1"));
    menu->addAction(icon, tr("action2"));
    menu->addAction(icon, tr("action3"));

    ExtensionButton *btn = new ExtensionButton(this);
    connect(tb, &QToolBar::orientationChanged, btn, &ExtensionButton::setOrientation);
    btn->setMenu(menu);
    tb->addWidget(btn);

    tb->addAction(icon, tr("act2"));

    setWindowTitle(tr("ExtensionButton Example"));
    setWindowIcon(icon);
    resize(400, 200);
}

MainWindow::~MainWindow()
{
}

