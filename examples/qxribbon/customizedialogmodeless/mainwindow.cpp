#include "mainwindow.h"

#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbonpage.h"
#include "qxribbon/ribbongroup.h"
#include "qxribbon/ribboncustomizedialog.h"

#include <QAction>

QX_RIBBON_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : RibbonWindow(parent)
{
    QIcon icon(":/logo");

    RibbonBar *rb = ribbonBar();

    RibbonPage *page = rb->addPage(tr("page1"));
    RibbonGroup *group = page->addGroup(tr("group1"));
    group->addLargeAction(new QAction(icon, tr("La"), this));
    group->addMediumAction(new QAction(icon, tr("Ma1"), this));

    setCentralWidget(new RibbonCustomizeDialog(rb)); // modeless

    resize(800, 400);
}

MainWindow::~MainWindow()
{
}
