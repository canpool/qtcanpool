#include "mainwindow.h"

#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbonpage.h"
#include "qxribbon/ribbonpagecontext.h"

QX_RIBBON_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : RibbonWindow(parent)
{
    RibbonBar *rb = ribbonBar();

    for (int i = 1; i <= 8; ++i) {
        RibbonPageContext *pc = rb->addPageContext(tr("context%1").arg(i));
        pc->addPage(tr("page1"));
        pc->addPage(tr("page2"));
        rb->showPageContext(pc);
    }

    resize(800, 400);
}

MainWindow::~MainWindow()
{
}
