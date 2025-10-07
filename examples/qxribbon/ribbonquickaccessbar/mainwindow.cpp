#include "mainwindow.h"

#include <QMenu>
#include <QDebug>

#include "qxribbon/ribbonquickaccessbar.h"

QX_RIBBON_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QIcon icon(":/logo");
    RibbonQuickAccessBar *qab = new RibbonQuickAccessBar(this);
    qab->actionCustomizeButton()->setToolTip(tr("Customize Quick Access Bar"));
    QAction *actionRemove = new QAction(icon, tr("remove"), this);
    qab->addAction(actionRemove);
    qab->removeAction(actionRemove);
    delete actionRemove;
    QAction *actions[5];
    for (int i = 0; i < 5; ++i) {
        actions[i] = qab->addAction(icon, tr("action %1").arg(i + 1));
    }
    qDebug() << "1)" << qab->state() << qab->visibleCount();    // "11111" 5
    actions[1]->setVisible(false);
    qDebug() << "2)" << qab->state() << qab->visibleCount();    // "10111" 4
    qab->setActionVisible(actions[0], false);
    qab->setActionVisible(actions[3], false);
    qDebug() << "3)" << qab->state() << qab->visibleCount();    // "00101" 2
    qab->setState("10101");
    qDebug() << "4)" << qab->state() << qab->visibleCount();    // "10101" 3
    QAction *actionState = new QAction(tr("state"), this);
    connect(actionState, &QAction::triggered, this, [qab]() {
        qDebug() << qab->state();
    });
    connect(qab, &RibbonQuickAccessBar::showCustomizeMenu, this, [actionState](QMenu *menu) {
        menu->addSeparator();
        menu->addAction(actionState);
    });
    addToolBar(qab);

    setWindowTitle(tr("RibbonQuickAccessBar Example"));
    setWindowIcon(icon);
    resize(400, 200);
}

MainWindow::~MainWindow()
{
}

