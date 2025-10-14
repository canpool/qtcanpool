#include "mainwindow.h"

#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbonpage.h"
#include "qxribbon/ribbonmenu.h"
#include "qxribbon/ribbonbuttongroup.h"

#include <QActionGroup>
#include <QAction>

QX_RIBBON_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : RibbonWindow(parent)
{
    QIcon icon(":/logo");

    RibbonBar *rb = ribbonBar();
    rb->addPage(tr("page1"));
    rb->addPage(tr("loooooooooooongpage2"));
    rb->addPage(tr("p3"));
    rb->addPage(tr("page4"));

    RibbonButtonGroup *rightBar = rb->cornerButtonGroup(Qt::BottomRightCorner);
    QActionGroup *styleGroup = new QActionGroup(this);
    QAction *actionTheme = new QAction(icon, tr("style"), this);
    RibbonMenu *menu = new RibbonMenu(tr("style"), this);
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    menu->setObjectName(QLatin1String("qx_compatible"));
#endif
    actionTheme->setMenu(menu);
    styleGroup->addAction(addStyleAction(menu->addAction(tr("office style")), RibbonBar::OfficeStyle));
    styleGroup->addAction(addStyleAction(menu->addAction(tr("wps style")), RibbonBar::WpsLiteStyle));
    styleGroup->addAction(addStyleAction(menu->addAction(tr("office 2row style")), RibbonBar::OfficeStyleTwoRow));
    styleGroup->addAction(addStyleAction(menu->addAction(tr("wps 2row style")), RibbonBar::WpsLiteStyleTwoRow));
    rightBar->addAction(actionTheme);
    styleGroup->actions().at(2)->trigger();

    resize(800, 400);
}

MainWindow::~MainWindow()
{
}

QAction *MainWindow::addStyleAction(QAction *action, int styleId)
{
    action->setCheckable(true);
    action->setData(QVariant(styleId));
    connect(action, &QAction::triggered, this, &MainWindow::onActionChangeStyleTriggered);

    return action;
}

void MainWindow::onActionChangeStyleTriggered()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        int id = action->data().toInt();
        ribbonBar()->setRibbonStyle(static_cast<RibbonBar::RibbonStyle>(id));
    }
}

