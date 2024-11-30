#include "mainwindow.h"

#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbonpage.h"
#include "qxribbon/ribbongroup.h"
#include "qxribbon/ribbonbuttongroup.h"

#include <QAction>
#include <QToolButton>
#include <QDebug>

QX_RIBBON_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : RibbonWindow(parent)
{
    QIcon icon(":/logo");

    RibbonBar *rb = ribbonBar();

    RibbonPage *page = rb->addPage(tr("page1"));
    RibbonGroup *group = page->addGroup(tr("group1"));

    RibbonButtonGroup *bg = new RibbonButtonGroup(this);
    bg->addAction(new QAction(icon, tr("action1"), this));
    bg->addAction(tr("action2"), icon);

    bg->addSeparator();

    QToolButton *btn = new QToolButton(this);
    btn->setText(tr("toolbutton"));
    btn->setIcon(icon);
    btn->setAutoRaise(true);
    bg->addWidget(btn);

    QMenu *menu = new QMenu(this);
    menu->addAction(tr("action1"));
    menu->addAction(tr("action2"));
    QAction *action = bg->addMenu(menu);
    action->setIcon(icon);

    // remove
    QAction *delAct = bg->addAction(new QAction(icon, tr("deleteaction1"), this));
    bg->removeAction(delAct);     // not deleted
    qDebug() << delAct->text();   // deleteaction1
    delAct = bg->addAction(tr("action2"), icon);
    bg->removeAction(delAct);   // deleted
    // delAct->text(); // programe crashed

    menu = new QMenu(tr("deletemenu1"), this);
    menu->addAction(tr("action1"));
    menu->addAction(tr("action2"));
    delAct = bg->addMenu(menu);
    delAct->setIcon(icon);
    bg->removeAction(delAct);     // not deleted
    qDebug() << delAct->text();   // deletemenu1

    group->addMediumWidget(bg);

    rb->addPage(tr("loooooooooooongpage2"));

    resize(800, 400);
}

MainWindow::~MainWindow()
{
}
