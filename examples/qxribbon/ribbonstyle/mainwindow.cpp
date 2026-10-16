#include "mainwindow.h"

#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbonpage.h"
#include "qxribbon/ribbonmenu.h"
#include "qxribbon/ribbonbuttongroup.h"
#include "qxribbon/ribbontheme.h"

#include <QActionGroup>
#include <QAction>

QX_RIBBON_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : RibbonMainWindow(parent)
{
    QIcon icon(":/logo");

    RibbonBar *rb = ribbonBar();
    rb->addPage(tr("page1"));
    rb->addPage(tr("loooooooooooongpage2"));
    rb->addPage(tr("p3"));
    rb->addPage(tr("page4"));

    RibbonButtonGroup *rightBar = rb->cornerButtonGroup(Qt::BottomRightCorner);

    // --- layout style (Office / Wps / two-row) ---
    QActionGroup *styleGroup = new QActionGroup(this);
    QAction *actionStyle = new QAction(icon, tr("style"), this);
    RibbonMenu *styleMenu = new RibbonMenu(tr("style"), this);
    actionStyle->setMenu(styleMenu);
    styleGroup->addAction(addStyleAction(styleMenu->addAction(tr("office style")), RibbonBar::OfficeStyle));
    styleGroup->addAction(addStyleAction(styleMenu->addAction(tr("wps style")), RibbonBar::WpsLiteStyle));
    styleGroup->addAction(addStyleAction(styleMenu->addAction(tr("office 2row style")), RibbonBar::OfficeStyleTwoRow));
    styleGroup->addAction(addStyleAction(styleMenu->addAction(tr("wps 2row style")), RibbonBar::WpsLiteStyleTwoRow));
    rightBar->addAction(actionStyle);
    styleGroup->actions().at(2)->trigger();

    // --- color theme, including the 4.5 Fluent themes (B7) ---
    QActionGroup *themeGroup = new QActionGroup(this);
    QAction *actionTheme = new QAction(icon, tr("theme"), this);
    RibbonMenu *themeMenu = new RibbonMenu(tr("theme"), this);
    actionTheme->setMenu(themeMenu);
    themeGroup->addAction(addThemeAction(themeMenu->addAction(tr("Office 2013")), RibbonTheme::LightOffice2013Theme));
    themeGroup->addAction(addThemeAction(themeMenu->addAction(tr("Fancy")), RibbonTheme::LightFancyTheme));
    themeGroup->addAction(addThemeAction(themeMenu->addAction(tr("Dark Office Plus")), RibbonTheme::DarkOfficePlusTheme));
    themeGroup->addAction(addThemeAction(themeMenu->addAction(tr("Fluent 2016")), RibbonTheme::LightFluent2016Theme));
    themeGroup->addAction(addThemeAction(themeMenu->addAction(tr("Fluent 2019")), RibbonTheme::LightFluent2019Theme));
    rightBar->addAction(actionTheme);
    themeGroup->actions().at(0)->trigger();

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

QAction *MainWindow::addThemeAction(QAction *action, int themeId)
{
    action->setCheckable(true);
    action->setData(QVariant(themeId));
    connect(action, &QAction::triggered, this, &MainWindow::onActionChangeThemeTriggered);
    return action;
}

void MainWindow::onActionChangeThemeTriggered()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        setRibbonTheme(action->data().toInt());
}
