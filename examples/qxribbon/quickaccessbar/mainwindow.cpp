#include "mainwindow.h"

#include <QMenu>
#include <QDebug>

#include "qxribbon/quickaccessbar.h"

QX_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QIcon icon(":/logo");
    QuickAccessBar *qab = new QuickAccessBar(this);
    qab->actionCustomizeButton()->setToolTip(tr("Customize Quick Access Bar"));
    QAction *actionRemove = new QAction(icon, tr("remove"), this);
    qab->addAction(actionRemove);
    qab->removeAction(actionRemove);
    delete actionRemove;
    for (int i = 0; i < 5; ++i) {
        qab->addAction(icon, tr("action %1").arg(i + 1));
    }
    qab->setState("10101");
    qDebug() << qab->state() << qab->visibleCount();
    QAction *actionState = new QAction(tr("state"), this);
    connect(actionState, &QAction::triggered, this, [qab]() {
        qDebug() << qab->state();
    });
    connect(qab, &QuickAccessBar::showCustomizeMenu, this, [actionState](QMenu *menu) {
        menu->addSeparator();
        menu->addAction(actionState);
    });
    addToolBar(qab);

    setWindowTitle(tr("QuickAccessBar Example"));
    setWindowIcon(icon);
    resize(400, 200);
}

MainWindow::~MainWindow()
{
}

