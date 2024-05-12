#include <QApplication>
#include <QDesktopWidget>
#include <QFontComboBox>
#include "aboutdialog.h"
#include "mainwindow.h"

/* MainWindow */
MainWindow::MainWindow(QWidget* parent)
    : RibbonWindow(parent)
{
    setStyleTheme(OfficeStyle::Office2007Blue);

    RibbonPage* page = ribbonBar()->addPage(tr("&Home"));

    createLargeButtons(page);
    createSmallButtons(page);
    createLargePlusSmallButtons(page);
    ribbonBar()->setTitleBarVisible(false);

    move(QPoint(200, 200));
    resize(1000, 300);
}

MainWindow::~MainWindow()
{
}

void MainWindow::createLargeButtons(RibbonPage* page)
{
    Qtitan::RibbonGroup* group = page->addGroup(tr("Large Buttons"));

    RibbonToolBarControl* toolBar = new RibbonToolBarControl(group);
    toolBar->addAction(QIcon(":/res/largeNewFile.png"), tr("New"), Qt::ToolButtonTextUnderIcon);
    toolBar->addAction(QIcon(":/res/largeOpenFile.png"), tr("Open"), Qt::ToolButtonTextUnderIcon);
    toolBar->addAction(QIcon(":/res/largePrint.png"), tr("Print"), Qt::ToolButtonTextUnderIcon);
    toolBar->addSeparator();
    toolBar->addAction(QIcon(":/res/largezoom.png"), tr("Zoom"), Qt::ToolButtonTextUnderIcon);
    toolBar->addAction(QIcon(":/res/largeWatermark.png"), tr("Watermark"), Qt::ToolButtonTextUnderIcon);

    group->addControl(toolBar);
}

void MainWindow::createSmallButtons(RibbonPage* page)
{
    Qtitan::RibbonGroup* group = page->addGroup(tr("Small Buttons"));
    RibbonToolBarControl* toolBar = new RibbonToolBarControl(group);

    QFontComboBox* fontComboBox = new QFontComboBox();
    fontComboBox->setMaximumWidth(130);
    toolBar->addWidget(fontComboBox);

    QComboBox* sizeComboBox = new QComboBox();
    sizeComboBox->setMaximumWidth(45);
    sizeComboBox->setEditable(true);
    QFontDatabase db;
    foreach(int size, db.standardSizes())
        sizeComboBox->addItem(QString::number(size));
    toolBar->addWidget(sizeComboBox);
    toolBar->addSeparator();

    toolBar->addAction(QIcon(":/res/smallnew.png"), tr("New"));
    toolBar->addAction(QIcon(":/res/smallOpen.png"), tr("Open"));
    toolBar->addAction(QIcon(":/res/smallPrint.png"), tr("Print"));
    toolBar->addSeparator();
    toolBar->addAction(QIcon(":/res/smallRedo.png"), tr("Redo"));
    toolBar->addAction(QIcon(":/res/smallUndo.png"), tr("Undo"));
    toolBar->addAction(QIcon(":/res/smallreplace.png"), tr("Replace"));
    toolBar->addSeparator();
    toolBar->addAction(QIcon(":/res/smallfontshrink.png"), tr("Shrink"));
    toolBar->addAction(QIcon(":/res/smallfontgrow.png"), tr("Grow"));

    group->addControl(toolBar);
}

void MainWindow::createLargePlusSmallButtons(RibbonPage* page)
{
    Qtitan::RibbonGroup* group = page->addGroup(tr("Large+Small Buttons"));

    RibbonToolBarControl* toolBar = new RibbonToolBarControl(group);
#if 0
    toolBar->addAction(QIcon(":/res/largeNewFile.png"), tr("New"), Qt::ToolButtonTextUnderIcon);
    toolBar->addAction(QIcon(":/res/largeOpenFile.png"), tr("Open"), Qt::ToolButtonTextUnderIcon);
    toolBar->addAction(QIcon(":/res/largePrint.png"), tr("Print"), Qt::ToolButtonTextUnderIcon);
    toolBar->addSeparator();
#endif
    toolBar->addAction(QIcon(":/res/largezoom.png"), tr("Zoom"), Qt::ToolButtonTextUnderIcon);
    toolBar->addAction(QIcon(":/res/largeWatermark.png"), tr("Watermark"), Qt::ToolButtonTextUnderIcon);
    toolBar->addSeparator();

    toolBar->addAction(QIcon(":/res/smallnew.png"), tr("New"));
    toolBar->addAction(QIcon(":/res/smallOpen.png"), tr("Open"));
    toolBar->addAction(QIcon(":/res/smallPrint.png"), tr("Print"));
    toolBar->addSeparator();
    toolBar->addAction(QIcon(":/res/smallRedo.png"), tr("Redo"));
    toolBar->addAction(QIcon(":/res/smallUndo.png"), tr("Undo"));
    toolBar->addAction(QIcon(":/res/smallreplace.png"), tr("Replace"));
    toolBar->addSeparator();
    toolBar->addAction(QIcon(":/res/smallfontshrink.png"), tr("Shrink"));
    toolBar->addAction(QIcon(":/res/smallfontgrow.png"), tr("Grow"));
    toolBar->addSeparator();

    group->addControl(toolBar);

    group->addSeparator();
    group->addAction(QIcon(":/res/largeNewFile.png"), tr("New"), Qt::ToolButtonTextUnderIcon);
    group->addAction(QIcon(":/res/largeOpenFile.png"), tr("Open"), Qt::ToolButtonTextUnderIcon);
    group->addAction(QIcon(":/res/largePrint.png"), tr("Print"), Qt::ToolButtonTextUnderIcon);
}

