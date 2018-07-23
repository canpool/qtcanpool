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
#include "cmainwindow.h"

#include "cwidgets.h"
#include "chartsmode.h"
#include "custommode.h"
#include "menumode.h"

#include "qcanpool/fancytabwidget.h"
#include "qcanpool/fancytabbar.h"
#include "qcanpool/modemanager.h"

#include "licensedialog.h"
#include "license/nrlicense.h"

#define COMPANY_NAME    QString::fromUtf8("QT FRAMEWORK")

CMainWindow *CMainWindow::m_instance = 0;

CMainWindow::CMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_instance = this;

    m_modeStack = new FancyTabWidget(this);
    m_modeManager = new ModeManager(m_modeStack);
    m_statusBar = m_modeStack->statusBar();

    createWindow();

    setAcceptDrops(true);

    setWindowTitle(tr("Customized Window"));
    setWindowIcon(QIcon(":/main/logo"));
    setMinimumSize(QSize(600,400));
    setMouseTracking(true);

    QRect geom = QApplication::desktop()->availableGeometry();
    resize(2 * geom.width() / 3, 2 * geom.height() / 3);
    raise();
    activateWindow();
}

CMainWindow::~CMainWindow()
{
    delete m_modeManager;
    deleteOthers();
}

CMainWindow *CMainWindow::instance()
{
    return m_instance;
}

void CMainWindow::checkLicense()
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

void CMainWindow::createWindow()
{
    createToolBar();
    createStatusBar();
    createDockWidget();
    createCentralWidget();
    createModeBar();

    createOthers();
}

void CMainWindow::createToolBar()
{
    setContextMenuPolicy(Qt::NoContextMenu);

//    m_pMainToolBar = addToolBar(tr("Main Toolbar"));
//    m_pMainToolBar->setIconSize(QSize(24,24));
////    m_pMainToolBar->setMovable(false);

//    QAction* smallButton = new QAction(QIcon(":/tools/new"), tr("New"));
//    smallButton->setShortcut(QKeySequence::New);
//    smallButton->setToolTip(tr("New File or Project\nCtrl+N"));
//    smallButton->setStatusTip(tr("New File or Project"));
//    m_pNewAction = smallButton;

//    smallButton = new QAction(QIcon(":/tools/open"), tr("Open"));
//    smallButton->setShortcut(QKeySequence::Open);
//    smallButton->setToolTip(tr("Open File or Project\nCtrl+O"));
//    smallButton->setStatusTip(tr("Open File or Project"));
//    m_pOpenAction = smallButton;

//    smallButton = new QAction(QIcon(":/tools/recent"), tr("Recent"));
//    smallButton->setToolTip(tr("Recent Files"));
//    smallButton->setStatusTip(tr("Recent Files"));
//    m_pRecentAction = smallButton;

//    smallButton = new QAction(QIcon(":/tools/save"), tr("Save"));
//    smallButton->setShortcut(QKeySequence::Save);
//    smallButton->setToolTip(tr("Save the active document\nCtrl+S"));
//    smallButton->setStatusTip(tr("Save the active document"));
//    m_pSaveAction = smallButton;

//    smallButton = new QAction(QIcon(":/tools/save_all"), tr("Save All"));
//    smallButton->setShortcut(tr("Ctrl+Shift+S"));
//    smallButton->setToolTip(tr("Save all active documents\nCtrl+Shift+S"));
//    smallButton->setStatusTip(tr("Save all active documents"));
//    m_pSaveAllAction = smallButton;

//    m_pMainMenuAction = new QAction(QIcon(":/tools/menu"),tr("menu"));

//    m_pMainToolBar->addAction(m_pNewAction);
//    m_pMainToolBar->addAction(m_pOpenAction);
//    m_pMainToolBar->addAction(m_pRecentAction);
//    m_pMainToolBar->addAction(m_pSaveAction);
//    m_pMainToolBar->addAction(m_pSaveAllAction);

//    m_pTailToolBar = addToolBar(tr("Tail Toolbar"));
//    QWidget *spacer = new QWidget();
//    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
//    m_pTailToolBar->addWidget(spacer);
//    m_pTailToolBar->addAction(m_pMainMenuAction);
}


void CMainWindow::createDockWidget()
{
//    m_pStandbyExplorer = new CStandbyExplorer(this);
//    connect(m_pStandbyExplorer, SIGNAL(sigAppendSignals(QStringList)), this, SLOT(slotAppendSignals(QStringList)));
//    QDockWidget *pDockWidgetTest = new QDockWidget(tr("Standby Explorer"), this);
//    pDockWidgetTest->setAllowedAreas(Qt::RightDockWidgetArea);
//    pDockWidgetTest->setWidget(m_pStandbyExplorer);
//    addDockWidget(Qt::RightDockWidgetArea, pDockWidgetTest);
//    pDockWidgetTest->setStyleSheet("color:black;");
//    pDockWidgetTest->setFeatures(QDockWidget::NoDockWidgetFeatures);
//    QAction *action = new QAction(tr("Standby Action"), this);
//    action->setCheckable(true);
//    connect(action, SIGNAL(toggled(bool)), pDockWidgetTest, SLOT(setHidden(bool)));
//    Global::standbyAction = action;
//    Global::setStandbyShown(false);

//    QTableView *m_pTableView = new QTableView(this);
//    QDockWidget *pDockWidgetScd = new QDockWidget(tr("SCD View"), this);
//    pDockWidgetScd->setAllowedAreas(Qt::AllDockWidgetAreas);
//    pDockWidgetScd->setWidget(m_pTableView);
//    //pDockWidgetScd->hide();
//    addDockWidget(Qt::RightDockWidgetArea, pDockWidgetScd);

//    QTextEdit* m_pTextEdit = new QTextEdit(this);
//    QDockWidget *pDockWidgetOutput = new QDockWidget(tr("Output"), this);
//    pDockWidgetOutput->setAllowedAreas(Qt::AllDockWidgetAreas);
//    pDockWidgetOutput->setWidget(m_pTextEdit);
//    addDockWidget(Qt::RightDockWidgetArea, pDockWidgetOutput);
}

void CMainWindow::createStatusBar()
{
//    m_pStatusBar = new QStatusBar();

//    m_pNavStackedWidget = new QStackedWidget();
//    m_pNavStackedWidget->setMinimumHeight(100);
//    m_pNavBar = new INavBar(m_pNavStackedWidget);
//    m_pNavBar->setSideExpand(true);
//    connect(m_pNavBar, SIGNAL(sigSideExpand(bool)), this, SLOT(slotSideExpand(bool)));
//    ApplicationOutput *appOutput = new ApplicationOutput();
//    m_pNavBar->add(tr("Application Output"), appOutput);

//    m_pNavBar->setTextColor(QColor(10,10,10));
//    m_pNavBar->setHoverColor(QColor(180,180,180));

//    m_pStatusBar->addWidget(m_pNavBar);

//    m_pCompanyLabel = new QLabel(COMPANY_NAME);
//    m_pStatusBar->addPermanentWidget(m_pCompanyLabel);
}

void CMainWindow::createCentralWidget()
{
//    QWidget *pCentralWidget = new QWidget();

//    QStackedWidget *pStackedWidget = new QStackedWidget();
//    pStackedWidget->setMinimumWidth(200);
//    QPalette palette;
////    palette.setBrush(QPalette::Window, QBrush(Qt::darkGray));
////    palette.setColor(QPalette::Background, QColor(255,255,255,230));
//    palette.setColor(QPalette::Background, QColor(240,240,240,240));
//    pStackedWidget->setPalette(palette);
//    pStackedWidget->setAutoFillBackground(true);

//    m_pModeBar = new IModeBar(pStackedWidget, IModeBar::Vertical, this);
////    QPalette palette;
//    palette.setColor(QPalette::Background, QColor(0,0,0,20));
//    m_pModeBar->setPalette(palette);
//    m_pModeBar->setAutoFillBackground(true);
//    createModeBar();

//    m_pExtendStackedWidget = new QStackedWidget();
//    m_pExtendStackedWidget->setMinimumWidth(100);
//    m_pStandbyExplorer = new CStandbyExplorer(this);
//    connect(m_pStandbyExplorer, SIGNAL(sigAppendSignals(QStringList)), this, SLOT(slotAppendSignals(QStringList)));
//    m_pExtendStackedWidget->addWidget(m_pStandbyExplorer);
//    QAction *action = new QAction(tr("Standby Action"), this);
//    action->setCheckable(true);
//    connect(action, SIGNAL(toggled(bool)), m_pExtendStackedWidget, SLOT(setHidden(bool)));
//    Global::standbyAction = action;
//    Global::setStandbyShown(false);

//    QSplitter *pTopSplitter = new QSplitter(Qt::Horizontal, this);
//    pTopSplitter->setMinimumHeight(100);
//    pTopSplitter->setChildrenCollapsible(false);
//    pTopSplitter->addWidget(pStackedWidget);
//    pTopSplitter->addWidget(m_pExtendStackedWidget);
//    pTopSplitter->setHandleWidth(1);
//    pTopSplitter->setStretchFactor(0,1);
//    pTopSplitter->setStyleSheet("QSplitter::handle { background-color: rgb(49,135,86);}");

//    QSplitter *pMainSplitter = new QSplitter(Qt::Vertical, this);
//    pMainSplitter->setChildrenCollapsible(false);
//    pMainSplitter->addWidget(pTopSplitter);
//    pMainSplitter->addWidget(m_pNavStackedWidget);
//    pMainSplitter->setHandleWidth(1);
//    pMainSplitter->setStretchFactor(0,1);
//    pMainSplitter->setStyleSheet("QSplitter::handle { background-color: rgb(49,135,86);}");

//    QWidget *line = new QWidget();
//    line->setFixedHeight(1);;
//    line->setStyleSheet("background-color: rgb(185,185,185);");

//    QVBoxLayout *pMainLayot = new QVBoxLayout();
//    pMainLayot->addWidget(pMainSplitter);
//    pMainLayot->addWidget(line);
//    pMainLayot->addWidget(m_pStatusBar);
//    pMainLayot->setSpacing(0);
//    pMainLayot->setContentsMargins(0,0,1,0);


//    QHBoxLayout *pCentralLayout = new QHBoxLayout();
//    pCentralLayout->addWidget(m_pModeBar);
//    pCentralLayout->addLayout(pMainLayot);
//    pCentralLayout->setSpacing(0);
//    pCentralLayout->setContentsMargins(0,0,1,0);

//    pCentralWidget->setLayout(pCentralLayout);
//    setCentralWidget(pCentralWidget);

    setCentralWidget(m_modeStack);
}

void CMainWindow::createModeBar()
{
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(0,0,0,20));

    FancyTabBar *modeBar = m_modeStack->tabBar();
    modeBar->setPalette(palette);
    modeBar->setAutoFillBackground(true);

    m_pChartsMode = new ChartsMode();
    m_pCustomMode = new CustomMode();
    m_pMenuMode = new MenuMode();

    m_modeManager->objectAdded(m_pCustomMode);
    m_modeManager->objectAdded(m_pChartsMode);
    m_modeManager->objectAdded(m_pMenuMode);

    m_modeManager->setCurrentMode(m_pCustomMode);

//    m_modeManager->setEnabled(m_pChartsMode, false);

    // mode action
    QAction *action = new QAction(QIcon(":/tools/start"), tr("Start"),this);
    action->setShortcut(tr("Ctrl+R"));
    action->setToolTip(tr("Start <i>Ctrl+R</i>"));
    action->setEnabled(false);
    modeBar->addAction(action);

    action = new QAction(QIcon(":/tools/suspend"), tr("Suspend"),this);
    modeBar->addAction(action);
    action->setEnabled(false);
    action = new QAction(QIcon(":/tools/stop"), tr("Stop"),this);
    modeBar->addAction(action);
    action->setEnabled(false);

    modeBar->setTabIconSize(QSize(24,24));
    modeBar->setTextColor(QColor(10,10,10));
    modeBar->setSelectedTextColor(QColor(255,255,255));
    modeBar->setHoverColor(QColor(180,180,180));
//    // mode
//    m_pEditMode = new EditMode();
//    m_pDebugMode = new DebugMode();
//    m_pReferMode = new ReferMode();
//    m_pGraphMode = new GraphMode();
//    m_pMasterMode = new MasterMode();
//    m_pExplorerMode = new ExplorerMode();

//    m_pModeBar->addMode(m_pEditMode);
//    m_pModeBar->addMode(m_pDebugMode);
//    m_pModeBar->addMode(m_pReferMode);
//    m_pModeBar->addMode(m_pGraphMode);
//    m_pModeBar->addMode(m_pMasterMode);
//    m_pModeBar->addMode(m_pExplorerMode);

//    m_pModeBar->selectMode(m_pEditMode);
////    m_pModeBar->selectMode(m_pExplorerMode);
//    m_pModeBar->setEnabled(m_pReferMode,false);
//    m_pModeBar->setEnabled(m_pDebugMode,false);
//    m_pModeBar->setVisible(m_pGraphMode, false);
////    m_pModeBar->setVisible(m_pMasterMode, false);

//    // mode action
//    QAction *action = new QAction(QIcon(":/tools/start"), tr("Start"),this);
//    action->setShortcut(tr("Ctrl+R"));
//    action->setToolTip(tr("Start <i>Ctrl+R</i>"));
//    action->setEnabled(false);
//    m_pModeBar->addAction(action);
//    m_pDebugMode->addAction(action);

//    action = new QAction(QIcon(":/tools/suspend"), tr("Suspend"),this);
//    m_pModeBar->addAction(action);
//    action->setEnabled(false);
//    m_pDebugMode->addAction(action);
//    action = new QAction(QIcon(":/tools/stop"), tr("Stop"),this);
//    m_pModeBar->addAction(action);
//    action->setEnabled(false);
//    m_pDebugMode->addAction(action);

//    m_pModeBar->setIconSize(QSize(24,24));
//    m_pModeBar->setTextColor(QColor(10,10,10));
//    m_pModeBar->setSelectedTextColor(QColor(255,255,255));
//    m_pModeBar->setHoverColor(QColor(180,180,180));
}

void CMainWindow::createOthers()
{

}

void CMainWindow::deleteOthers()
{
//    if(m_pMainMenu){
//        delete m_pMainMenu;
//        m_pMainMenu = 0;
//    }
//    if(m_pRecentProjectMenu){
//        delete m_pRecentProjectMenu;
//        m_pRecentProjectMenu = 0;
//    }
}

void CMainWindow::createConnects()
{
//    connect(m_pMainMenuAction, SIGNAL(triggered(bool)), this, SLOT(slotMainMenu()));
//    //
//    connect(m_pNewAction, SIGNAL(triggered(bool)), m_pEditMode, SIGNAL(sigNewFile()));
//    connect(m_pOpenAction, SIGNAL(triggered(bool)), this, SLOT(slotOpen()));
//    connect(m_pSaveAction, SIGNAL(triggered(bool)), m_pEditMode, SLOT(slotSave()));
//    connect(m_pSaveAllAction, SIGNAL(triggered(bool)), m_pEditMode, SLOT(slotSaveAll()));
//    connect(this, SIGNAL(sigSaveAll()), m_pEditMode, SLOT(slotSaveAll()));
//    connect(m_pSaveAction, SIGNAL(triggered(bool)), m_pExplorerMode, SLOT(slotSave()));
//    connect(m_pSaveAllAction, SIGNAL(triggered(bool)), m_pExplorerMode, SLOT(slotSaveAll()));
//    connect(this, SIGNAL(sigSaveAll()), m_pExplorerMode, SLOT(slotSaveAll()));
//    connect(this, SIGNAL(sigAddFile(QString)), m_pEditMode, SIGNAL(sigAddFile(QString)));

//    connect(m_pRecentAction, SIGNAL(triggered(bool)), this, SLOT(slotRecent()));

//    connect(m_pEditMode, SIGNAL(sigProjectState(int)), this, SLOT(slotProjectState(int)));
//    connect(m_pEditMode, SIGNAL(sigProjectState(int)), m_pReferMode, SLOT(slotProjectState(int)));
//    connect(m_pEditMode, SIGNAL(sigProjectState(int)), m_pDebugMode, SLOT(slotProjectState(int)));

//    connect(m_pModeBar, SIGNAL(currentChanged(int)), this, SLOT(slotModeCurrentChanged(int)));
}

void CMainWindow::readSettings()
{
    QSettings settings("Canpool", "qtproject");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    move(pos);
    resize(size);
}

void CMainWindow::writeSettings()
{
    QSettings settings("Canpool", "qtproject");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}
