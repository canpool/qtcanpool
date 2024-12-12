#include "mainwindow.h"

#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbonpage.h"
#include "qxribbon/ribbongroup.h"
#include "qxribbon/ribbonbutton.h"
#include "qxribbon/ribbonmenu.h"

#include <QAction>
#include <QToolBar>

QX_RIBBON_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : RibbonWindow(parent)
{
    QIcon icon(":/logo");

    RibbonBar *rb = ribbonBar();

    RibbonPage *page = rb->addPage(tr("page1"));
    RibbonGroup *group = page->addGroup(tr("group1"));
    RibbonButton *btn = group->addLargeAction(new QAction(icon, tr("La"), this));
    btn->setPopupMode(QToolButton::InstantPopup);
    RibbonMenu *menu = new RibbonMenu(this);
    btn->setMenu(menu);
    QToolBar *tb = new QToolBar(this);
    for (int i = 1; i <= 5; ++i) {
        tb->addAction(icon, tr("action%1").arg(i));
    }
    menu->addAction(tr("action1"));
    menu->addAction(tr("action2"));
    menu->addWidget(tb);
    menu->addAction(tr("action3"));
    menu->addSeparator();
    menu->addAction(tr("action4"));
    connect(tb, &QToolBar::actionTriggered, this, [menu]() {
        menu->hide();
    });


    page = rb->addPage(tr("loooooooooooongpage2"));

    resize(800, 400);
}

MainWindow::~MainWindow()
{
}
