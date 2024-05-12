#include "mainwindow.h"

#include <QApplication>
#include <QDesktopWidget>

#include "aboutdialog.h"


/* MainWindow */
MainWindow::MainWindow(QWidget* parent)
    : RibbonWindow(parent)
{
    RibbonWorkspace* workspace = new RibbonWorkspace;
    setCentralWidget(workspace);
    createRibbon();
    createStatusBar();

    move(QPoint(200, 200));
    QRect geom = QApplication::desktop()->availableGeometry();
    resize(2 * geom.width() / 3, 2 * geom.height() / 3);
}

MainWindow::~MainWindow()
{
}

void MainWindow::createRibbon()
{
    RibbonPage* page = ribbonBar()->addPage(tr("Page"));
    RibbonGroup* group = page->addGroup(tr("Group"));
    group->addAction(QIcon(":/shared/res/qtitanlogo32x32.png"), tr("Button"), Qt::ToolButtonTextUnderIcon);

    QString strContextTitle(tr("Contextual Group"));
    QString strContextGroupName("id_contextual_group");
    RibbonPage* contextGroup1 = ribbonBar()->addPage(tr("Page 1"));
    group = contextGroup1->addGroup(tr("Group"));
    group->addAction(QIcon(":/shared/res/qtitanlogo32x32.png"), tr("Button 1"), Qt::ToolButtonTextUnderIcon);
    contextGroup1->setContextColor(RibbonPage::ContextColorRed);
    contextGroup1->setContextTitle(strContextTitle);
    contextGroup1->setContextGroupName(strContextGroupName);

    RibbonPage* contextGroup2 = ribbonBar()->addPage(tr("Page 2"));
    group = contextGroup2->addGroup(tr("Group"));
    group->addAction(QIcon(":/shared/res/qtitanlogo32x32.png"), tr("Button 2"), Qt::ToolButtonTextUnderIcon);
    contextGroup2->setContextColor(RibbonPage::ContextColorRed);
    contextGroup2->setContextTitle(strContextTitle);
    contextGroup2->setContextGroupName(strContextGroupName);

    page = ribbonBar()->addPage(tr("Page"));
    group = page->addGroup(tr("Group"));
    group->addAction(QIcon(":/shared/res/qtitanlogo32x32.png"), tr("Button"), Qt::ToolButtonTextUnderIcon);

    RibbonPage* context = ribbonBar()->addPage(tr("Page"));
    group = context->addGroup(tr("Group"));
    group->addAction(QIcon(":/shared/res/qtitanlogo32x32.png"), tr("Button"), Qt::ToolButtonTextUnderIcon);

    context->setContextColor(RibbonPage::ContextColorBlue);
    context->setContextTitle(tr("Contextual"));

    ribbonBar()->setFrameThemeEnabled();
}

void MainWindow::createStatusBar()
{
    RibbonSliderPane* sliderPane = new RibbonSliderPane();
    sliderPane->setScrollButtons(true);
    sliderPane->setRange(0, 100);
    sliderPane->setMaximumWidth(130);
    sliderPane->setSingleStep(10);
    sliderPane->setSliderPosition(50);
    statusBar()->addPermanentWidget(sliderPane);
    statusBar();
}
