/***************************************************************************
 **
 **  Copyright (C) 2018 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **
 **  This program is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  This program is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with this program.  If not, see http://www.gnu.org/licenses/.
 **
****************************************************************************/
#include "mainwindow.h"
#include "cwidgets.h"
#include "chartsmode.h"
#include "custommode.h"
#include "menumode.h"
#include "paintmode.h"

#include "qcanpool/fancytabwidget.h"
#include "qcanpool/fancytabbar.h"
#include "qcanpool/modemanager.h"
#include "qcanpool/fancybar.h"
#include "qcanpool/quickaccessbar.h"
#include "qcanpool/fancydialog.h"
#include "qcanpool/fancynavbar.h"
#include "qcanpool/pagesorter.h"
#include "qcanpool/fancybanner.h"

#include "skindialog.h"
#include "theme.h"

#include "licensedialog.h"
#include "license/nrlicense.h"

#define COMPANY_NAME    QString::fromUtf8("科技有限公司")

MainWindow *MainWindow::m_instance = 0;

MainWindow::MainWindow(QWidget *parent)
    : FancyWindow(parent)
{
    m_instance = this;
    m_styleAction = 0;
    m_styleActions.clear();

    Theme::init();

    m_modeStack = new FancyTabWidget(this);
    m_modeManager = new ModeManager(m_modeStack);
    m_statusBar = m_modeStack->statusBar();

    createWindow();
    createConnects();

    setAcceptDrops(true);

    setWindowTitle(tr("Qt canpool fancy demo"));
    setWindowIcon(QIcon(":/main/logo"));
    setMinimumSize(QSize(600,400));
    setMouseTracking(true);
	
	updateTheme();
}

MainWindow::~MainWindow()
{
    delete m_modeManager;
    deleteOthers();
}

MainWindow *MainWindow::instance()
{
    return m_instance;
}

void MainWindow::checkLicense()
{
    int nret = CheckLicense("fancydemo");
    if(nret > 0)
        return;

    LicenseDialog dlg(this);
    if(dlg.exec() == QDialog::Accepted)
    {
        ;
    }
    else
    {
        exit(1);
    }
}


void MainWindow::createWindow()
{
    createQuickAccessBar();
    createMenuBar();
    createToolBar();
    createStatusBar();
//    createDockWidget();
    createCentralWidget();
    createModeBar();
    createAdditionalControls();

    createOthers();
}

void MainWindow::createQuickAccessBar()
{
    if (QuickAccessBar* quickAccessBar = fancyBar()->quickAccessBar())
    {
        QAction* action = quickAccessBar->actionCustomizeButton();
        action->setToolTip(tr("Customize Quick Access Bar"));

        QAction* smallButton = quickAccessBar->addAction(QIcon(":/tools/new"), tr("New"));
        smallButton->setShortcut(QKeySequence::New);
        smallButton->setToolTip(tr("New File or Project\nCtrl+N"));
        connect(smallButton, SIGNAL(triggered()), this, SLOT(slotNew()));
//        quickAccessBar->setActionVisible(smallButton, false);

        smallButton = quickAccessBar->addAction(QIcon(":/tools/open"), tr("Open"));
        smallButton->setShortcut(tr("Ctrl+O"));
        smallButton->setToolTip(tr("Open File or Project\nCtrl+O"));
//        connect(smallButton, SIGNAL(triggered()), this, SLOT(slotOpen()));
//        quickAccessBar->setActionVisible(smallButton, false);

        QMenu *menu = new QMenu(this);
        menu->addAction(tr("action1"));
        menu->addAction(tr("action2"));
        menu->addAction(tr("action3"));
        menu->addAction(tr("action4"));
        smallButton->setMenu(menu);

        smallButton = quickAccessBar->addAction(QIcon(":/tools/save"), tr("Save"));
        smallButton->setShortcut(tr("Ctrl+S"));
        smallButton->setToolTip(tr("Save the active document\nCtrl+S"));
        connect(smallButton, SIGNAL(triggered()), this, SLOT(slotSave()));

        smallButton = quickAccessBar->addAction(QIcon(":/tools/save_all"), tr("Save All"));
        smallButton->setShortcut(tr("Ctrl+Shift+S"));
        smallButton->setToolTip(tr("Save all active documents\nCtrl+Shift+S"));
//        connect(smallButton, SIGNAL(triggered()), this, SLOT(pressButton()));

        smallButton = quickAccessBar->addAction(QIcon(":/tools/undo"), tr("&Undo"));
        smallButton->setShortcut(QKeySequence::Undo);
        smallButton->setEnabled(false);

        smallButton = quickAccessBar->addAction(QIcon(":/tools/redo"), tr("&Redo"));
        smallButton->setShortcut(QKeySequence::Redo);
//        smallButton->setEnabled(false);

        fancyBar()->showQuickAccess(true);

        fancyBar()->setBackgroundColor(QColor(0, 100, 200, 100));
    }
}

void MainWindow::createMenuBar()
{
    if(QMenuBar* menuBar = fancyBar()->menuBar())
    {
        QAction *action = new QAction(tr("action1"),this);
        QMenu* menu = menuBar->addMenu(tr("&File"));
        menu->addAction(action);
        action = new QAction(tr("action1"),this);
        menu = menuBar->addMenu(tr("&Edit"));
        menu->addAction(action);
        action = new QAction(tr("action1"),this);
        menu = menuBar->addMenu(tr("&Debug"));
        menu->addAction(action);
        action = new QAction(tr("action1"),this);
        menu = menuBar->addMenu(tr("&View"));
        menu->addAction(action);
        action = new QAction(tr("action1"),this);
        menu = menuBar->addMenu(tr("&Window"));
        menu->addAction(action);
        action = new QAction(tr("action1"),this);
        menu = menuBar->addMenu(tr("&Tools"));
        menu->addAction(action);
        action = new QAction(tr("action1"),this);
        menu = menuBar->addMenu(tr("&Help"));
        menu->addAction(action);

        fancyBar()->showMenuBar(true);
    }

    QTextBrowser *text = new QTextBrowser(this);
    fancyBar()->setApplicationWidget(tr("Application"), text);
}

void MainWindow::createAdditionalControls()
{
    QAction *menuAction = new QAction(QIcon(":/tools/menu"), tr("main menu"), this);
    fancyBar()->addAdditionalControl(menuAction, FancyBar::TitlePosition);
    QAction *helpAction = new QAction(QIcon(":/tools/help"), tr("Help"), this);
    connect(helpAction, SIGNAL(triggered(bool)), this, SLOT(slotHelp()));
    fancyBar()->addAdditionalControl(helpAction, FancyBar::TitlePosition);
    QAction *skinAction = new QAction(QIcon(":/tools/skin"), tr("Skin"), this);
    connect(skinAction, SIGNAL(triggered(bool)), this, SLOT(slotSkin()));
    fancyBar()->addAdditionalControl(skinAction, FancyBar::TitlePosition);

    QMenu *menu = new QMenu();
    // style
    QAction *styleActionMenu = new QAction(tr("Style"));
    menu->addAction(styleActionMenu);
    QMenu *styleMenu = new QMenu(this);
    QActionGroup *actionGroup = new QActionGroup(this);
    QAction *styleAction = new QAction(tr("Window Style"));
    styleAction->setCheckable(true);
    styleMenu->addAction(styleAction);
    actionGroup->addAction(styleAction);
    m_styleActions.append(styleAction);
    styleAction = new QAction(tr("Classic Style"));
    styleAction->setCheckable(true);
    styleMenu->addAction(styleAction);
    actionGroup->addAction(styleAction);
    m_styleActions.append(styleAction);
    styleAction = new QAction(tr("Merged Style"));
    styleAction->setCheckable(true);
    styleMenu->addAction(styleAction);
    actionGroup->addAction(styleAction);
    m_styleActions.append(styleAction);
    styleActionMenu->setMenu(styleMenu);
    connect(actionGroup, SIGNAL(triggered(QAction*)), this, SLOT(slotSetStyle(QAction*)));
    m_styleActions.at(0)->setChecked(true);

    menu->addAction(tr("action2"));
    menu->addAction(tr("action3"));
    menu->addAction(tr("action4"));
    menuAction->setMenu(menu);

    QAction *action = new QAction(QIcon(":/tools/option"), tr("Option"), this);
    fancyBar()->addAdditionalControl(action, FancyBar::MenuPosition);

    QLineEdit *lineEdit = new QLineEdit();
    fancyBar()->addAdditionalControl(lineEdit, FancyBar::MenuPosition);
}

void MainWindow::createToolBar()
{
    setContextMenuPolicy(Qt::NoContextMenu);
}

void MainWindow::createStatusBar()
{
//    m_statusBar = m_modeStack->statusBar();

    m_pNavBar = new FancyNavBar();
    m_pNavBar->setSideExpand(true);
//    m_pNavBar->setVisible(false);
    QSplitter *splitter = m_modeStack->addCornerWidget(m_pNavBar->panel(), FancyTabWidget::Bottom);
    m_pNavBar->setSplitter(splitter);

    QTextBrowser *tb1 = new QTextBrowser(this);
    QTextBrowser *tb2 = new QTextBrowser(this);
    m_pNavBar->add(tr("tb1"), tb1);
    m_pNavBar->add(tr("tb2"), tb2, new QLineEdit());

//    m_pNavBar->remove(tb1);
//    delete tb1;

    m_statusBar->addWidget(m_pNavBar);
}

void MainWindow::createDockWidget()
{
    QTreeWidget *m_pTreeWidget = new QTreeWidget(this);
    m_pTreeWidget->setHeaderHidden(true);
    m_pTreeWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pTreeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pTreeWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
//    connect(m_pTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
//            this, SLOT(SlotItemActivated(QTreeWidgetItem*,int)));

    QDockWidget *pDockWidgetTest = new QDockWidget(tr("Test View"), this);
    pDockWidgetTest->setAllowedAreas(Qt::AllDockWidgetAreas);
    pDockWidgetTest->setWidget(m_pTreeWidget);
    addDockWidget(Qt::LeftDockWidgetArea, pDockWidgetTest);

    QTableView *m_pTableView = new QTableView(this);
    QDockWidget *pDockWidgetScd = new QDockWidget(tr("SCD View"), this);
    pDockWidgetScd->setAllowedAreas(Qt::AllDockWidgetAreas);
    pDockWidgetScd->setWidget(m_pTableView);
    //pDockWidgetScd->hide();
    addDockWidget(Qt::RightDockWidgetArea, pDockWidgetScd);

    QTextEdit* m_pTextEdit = new QTextEdit(this);
    QDockWidget *pDockWidgetOutput = new QDockWidget(tr("Output"), this);
    pDockWidgetOutput->setAllowedAreas(Qt::AllDockWidgetAreas);
    pDockWidgetOutput->setWidget(m_pTextEdit);
    addDockWidget(Qt::RightDockWidgetArea, pDockWidgetOutput);
}

void MainWindow::createCentralWidget()
{
    setCentralWidget(m_modeStack);

//    QTextBrowser *tb1 = new QTextBrowser(this);
//    QTextBrowser *tb2 = new QTextBrowser(this);
//    tb2->setFixedHeight(30);
//    QTextBrowser *tb3 = new QTextBrowser(this);
//    QTextBrowser *tb4 = new QTextBrowser(this);
//    m_modeStack->addCornerWidget(tb1, FancyTabWidget::Left);
//    m_modeStack->addCornerWidget(tb2, FancyTabWidget::Top);
//    m_modeStack->addCornerWidget(tb3, FancyTabWidget::Right);
//    m_modeStack->addCornerWidget(tb4, FancyTabWidget::Bottom);
}

void MainWindow::createModeBar()
{
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(0,0,0,20));

    FancyTabBar *modeBar = m_modeStack->tabBar();
    modeBar->setPalette(palette);
    modeBar->setAutoFillBackground(true);

    m_pChartsMode = new ChartsMode();
    m_pCustomMode = new CustomMode();
    m_pPaintMode = new PaintMode();
    m_pMenuMode = new MenuMode();

    m_modeManager->objectAdded(m_pCustomMode);
    m_modeManager->objectAdded(m_pPaintMode);
    m_modeManager->objectAdded(m_pChartsMode);
    m_modeManager->objectAdded(m_pMenuMode);

    m_modeManager->setCurrentMode(m_pCustomMode);

//    m_modeManager->setEnabled(m_pChartsMode, false);

    // mode action
    QAction *action = new QAction(QIcon(":/tools/start"), tr("Start"),this);
    action->setShortcut(tr("Ctrl+R"));
    action->setToolTip(tr("Start <i>Ctrl+R</i>"));
//    action->setEnabled(false);
    modeBar->addAction(action);

    action = new QAction(QIcon(":/tools/suspend"), tr("Suspend"),this);
    modeBar->addAction(action);
//    action->setEnabled(false);
    action = new QAction(QIcon(":/tools/stop"), tr("Stop"),this);
    modeBar->addAction(action/*, FancyTabBar::Front*/);
//    action->setEnabled(false);

    modeBar->setTabIconSize(QSize(24,24));
    modeBar->setTextColor(QColor(255,255,255));
    modeBar->setSelectedTextColor(QColor(255,255,255));
    modeBar->setHoverColor(QColor(255,255,255,50));

//    modeBar->setHeadSpace(5);

//    modeBar->setFixedWidth(200);
}

void MainWindow::createOthers()
{
}

void MainWindow::deleteOthers()
{

}

void MainWindow::createConnects()
{
    connect(this, SIGNAL(resizable(bool)), this, SLOT(slotResizable(bool)));
}

void MainWindow::updateTheme()
{
    FancyTabBar *modeBar = m_modeStack->tabBar();
    modeBar->setTextColor(Theme::g_fancyTabTextColor);
    modeBar->setSelectedTextColor(Theme::g_fancyTabSelectedTextColor);
    modeBar->setHoverColor(Theme::g_fancyTabHoverColor);
    modeBar->setPressColor(Theme::g_fancyTabPressColor);
    modeBar->setBackgroundColor(Theme::g_fancyTabBackgroundColor);

    fancyBar()->setHoverColor(Theme::g_fancyBarHoverColor);
    fancyBar()->setPressColor(Theme::g_fancyBarPressColor);
    fancyBar()->setTextColor(Theme::g_fancyBarTextColor);
    fancyBar()->setBackgroundColor(Theme::g_fancyBarBackgroundColor);
}

void MainWindow::slotHelp()
{
    QTextBrowser *tb = new QTextBrowser(this);
    FancyDialog dialog;
    dialog.setCentralWidget(tb);
    dialog.exec();
}

void MainWindow::slotNew()
{
//    QDialog dlg;
//    QHBoxLayout layout;
//    PageSorter sorter;
//    sorter.setMaxPage(5);
//    layout.addWidget(&sorter);
//    dlg.setLayout(&layout);
//    dlg.exec();

    FancyDialog dialog;
    dialog.setTitleBarHeight(30);
    dialog.addAdditionalControl(new QAction(QIcon(":/tools/new"), tr("Stop"),this));
    dialog.addAdditionalControl(new QAction(QIcon(":/tools/open"), tr("Stop"),this));
    dialog.addAdditionalControl(new QAction(QIcon(":/tools/save"), tr("Stop"),this));
    dialog.addAdditionalControl(new QAction(QIcon(":/tools/cut"), tr("Stop"),this));
    dialog.exec();
}

void MainWindow::slotSkin()
{
    SkinDialog dlg;
    connect(&dlg, SIGNAL(changeSkin(QString)), this, SLOT(slotChangeSkin(QString)));
    connect(&dlg, SIGNAL(themeChanged()), this, SLOT(updateTheme()));
    dlg.exec();
}

void MainWindow::slotSave()
{
    QDialog dialog;
    FancyBanner banner;
    for(int i = 0; i < 10; i++){
        banner.addPage(QPixmap(QString(":/skin/%1").arg(i)));
    }
    QHBoxLayout layout;
    layout.setMargin(0);
    layout.setSpacing(0);
    layout.addWidget(&banner);
    dialog.setLayout(&layout);
    dialog.resize(300, 80);
    dialog.exec();
}

void MainWindow::slotResizable(bool resizable)
{
    m_statusBar->setSizeGripEnabled(resizable);
}

void MainWindow::slotChangeSkin(QString skinName)
{
    QSettings settings("Canpool", "qtcanpool");
    settings.setValue("skin", skinName);
    update();
}

void MainWindow::slotSetStyle(QAction *action)
{
    if(m_styleAction == action){
        return;
    }
    m_styleAction = action;
    int index = m_styleActions.indexOf(action);
    fancyBar()->setFancyStyle((FancyBar::FancyStyle)index);
}

void MainWindow::readSettings()
{
    QSettings settings("Canpool", "qtproject");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    move(pos);
    resize(size);
}

void MainWindow::writeSettings()
{
    QSettings settings("Canpool", "qtproject");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}
