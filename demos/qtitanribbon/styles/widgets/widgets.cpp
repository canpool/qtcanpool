#include "widgets.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStyleFactory>
#include <QDockWidget>
#include <QMdiArea>
#include <QTableWidget>
#include <QMdiSubWindow>
#include <QSettings>
#include <QToolButton>
#include <QTextEdit>
#include <QDialog>

#include <aboutdialog.h>

#include <QtnRibbonStyle.h>

#include "ui_widgetsdlg.h"
#include "dockwidgets.h"


/* Widgets */
Widgets::Widgets(QWidget* parent)
    : QMainWindow(parent)
{
    m_officeStyle = Q_NULL;
    setWindowTitle(tr("Qtitan Widgets Sample"));

    m_styleName = qApp->style()->objectName();

    m_mdiArea = new QMdiArea;
    m_mdiArea->setViewMode(QMdiArea::TabbedView);
    m_mdiArea->setLineWidth(3);
    m_mdiArea->setFrameShape(QFrame::Panel);
    m_mdiArea->setFrameShadow(QFrame::Sunken);

    setCentralWidget(m_mdiArea);

    setIconSize(QSize(16, 16));

    createActions();
    createDockWindows();
    createMenubar();
    createToolbar();
    statusBar();

    createMdiChild();

    QRect geom = QApplication::desktop()->availableGeometry();
    resize(2 * geom.width() / 3, 2 * geom.height() / 3);

    updateStyle(m_actOffice2007Style);
    updateTheme(m_actBlueTheme);

    readSettings();
}

Widgets::~Widgets()
{
}

void Widgets::closeEvent(QCloseEvent* event)
{
    writeSettings();
    QMainWindow::closeEvent(event);
}

void Widgets::createActions()
{
    // Group File
    m_actNew = new QAction(QIcon(":/res/new.png"), tr("&New"), this);
    m_actNew->setStatusTip(tr("Create a new document"));
    m_actNew->setPriority(QAction::LowPriority);
    m_actNew->setShortcut(QKeySequence::New);
    connect(m_actNew, SIGNAL(triggered()), this, SLOT(createMdiChild()));

    m_actOpen = new QAction(QIcon(":/res/open.png"), tr("&Open..."), this);
    m_actOpen->setStatusTip(tr("Open an existing document"));
    m_actOpen->setShortcut(QKeySequence::Open);

    m_actSave = new QAction(QIcon(":/res/save.png"), tr("&Save"), this);
    m_actSave->setStatusTip(tr("Save the active document"));
    m_actSave->setShortcut(QKeySequence::Save);

    m_actClose = new QAction(tr("&Close"), this);
    m_actClose->setStatusTip(tr("Close the active document"));

    m_actSaveAs = new QAction(tr("Save &As..."), this);
    m_actSaveAs->setPriority(QAction::LowPriority);
    m_actSaveAs->setStatusTip(tr("Save the active document with a new name"));

#ifndef QT_NO_PRINTER
    m_actPrint = new QAction(QIcon(":/res/print.png"),tr("&Print..."), this);
    m_actPrint->setPriority(QAction::LowPriority);    
    m_actPrint->setShortcut(QKeySequence::Print);
    m_actPrint->setStatusTip(tr("Print the active document"));

    m_actPrintPreview = new QAction(QIcon(":/res/printpreview.png"),tr("Print Preview..."), this);
    m_actPrintPreview->setStatusTip(tr("Display full pages"));
#endif
    m_actExit = new QAction(tr("E&xit"), this);
    m_actExit->setStatusTip(tr("Quit the application; prompts to save documents"));
    connect(m_actExit, SIGNAL(triggered()), this, SLOT(close()));
    // Group styles
    QActionGroup* groupActions = new QActionGroup(this);
    connect(groupActions, SIGNAL(triggered(QAction*)), this, SLOT(triggeredStyles(QAction*)));

    m_actDefaultStyle = new QAction(tr("Default"), this);
    m_actDefaultStyle->setStatusTip(tr("Default Style"));
    m_actDefaultStyle->setCheckable(true);
    groupActions->addAction(m_actDefaultStyle);

    m_actOffice2007Style = new QAction(tr("Office2007"), this);
    m_actOffice2007Style->setStatusTip(tr("Office2007 Style"));
    m_actOffice2007Style->setCheckable(true);
    groupActions->addAction(m_actOffice2007Style);

    m_actRibbonStyle = new QAction(tr("Ribbon"), this);
    m_actRibbonStyle->setStatusTip(tr("Ribbon Style"));
    m_actRibbonStyle->setCheckable(true);
    groupActions->addAction(m_actRibbonStyle);

    // Group themes
    m_actToggleThemes = new QAction(tr("Themes"), this);
    m_groupThemesActions = new QActionGroup(this);
    connect(m_groupThemesActions, SIGNAL(triggered(QAction*)), this, SLOT(triggeredThemes(QAction*)));

    m_actBlueTheme = new QAction(tr("Office 2007 Blue"), this);
    m_actBlueTheme->setCheckable(true);
    m_groupThemesActions->addAction(m_actBlueTheme);

    m_actBlackTheme = new QAction(tr("Office 2007 Black"), this);
    m_actBlackTheme->setCheckable(true);
    m_groupThemesActions->addAction(m_actBlackTheme);

    m_actSilverTheme = new QAction(tr("Office 2007 Silver"), this);
    m_actSilverTheme->setCheckable(true);
    m_groupThemesActions->addAction(m_actSilverTheme);

    m_actAquaTheme = new QAction(tr("Office 2007 Aqua"), this);
    m_actAquaTheme->setCheckable(true);
    m_groupThemesActions->addAction(m_actAquaTheme);

    m_actScenicTheme = new QAction(tr("Windows 7 Scenic"), this);
    m_actScenicTheme->setCheckable(true);
    m_groupThemesActions->addAction(m_actScenicTheme);

    m_actBlue2010Theme = new QAction(tr("Office 2010 Blue"), this);
    m_actBlue2010Theme->setCheckable(true);
    m_groupThemesActions->addAction(m_actBlue2010Theme);

    m_actSilver2010Theme = new QAction(tr("Office 2010 Silver"), this);
    m_actSilver2010Theme->setCheckable(true);
    m_groupThemesActions->addAction(m_actSilver2010Theme);

    m_actBlack2010Theme = new QAction(tr("Office 2010 Black"), this);
    m_actBlack2010Theme->setCheckable(true);
    m_groupThemesActions->addAction(m_actBlack2010Theme);

    // Group help
    m_actHelp = new QAction(QIcon(":/shared/res/about.png"), tr("&About Custom Styles Sample..."), this);
    m_actHelp->setStatusTip(tr("Display program information, version number and copyright"));
    connect(m_actHelp, SIGNAL(triggered()), this, SLOT(about()));
}

void Widgets::createMenubar()
{
    QMenuBar* mainMenuBar = menuBar();
    QMenu* menuFile = new QMenu(tr("&File"), this);
    mainMenuBar->addMenu(menuFile);

    menuFile->addAction(m_actNew);
    menuFile->addAction(m_actOpen);
    menuFile->addAction(m_actClose);
    menuFile->addAction(m_actSave);
    menuFile->addAction(m_actSaveAs);

    menuFile->addSeparator();

#ifndef QT_NO_PRINTER
    menuFile->addAction(m_actPrint);

    menuFile->addAction(m_actPrintPreview);
    menuFile->addSeparator();
#endif
    menuFile->addAction(m_actExit);

    m_menuView = new QMenu(tr("&View"), this);
    mainMenuBar->addMenu(m_menuView);
    m_menuView->addAction(m_styleDock->toggleViewAction());
    m_menuView->addAction(m_propDock->toggleViewAction());
    m_menuView->addAction(m_toolBoxDock->toggleViewAction());
    m_menuView->addSeparator();
    m_menuView->addAction(m_actDefaultStyle);
    m_menuView->addAction(m_actOffice2007Style);
    m_menuView->addAction(m_actRibbonStyle);
    m_menuView->addSeparator();
    m_menuTheme = new QMenu(tr("&Theme"), this); 
    m_menuTheme->addAction(m_actBlueTheme);
    m_menuTheme->addAction(m_actBlackTheme);
    m_menuTheme->addAction(m_actSilverTheme);
    m_menuTheme->addAction(m_actAquaTheme);
    m_menuTheme->addAction(m_actScenicTheme);

    m_menuTheme->addAction(m_actBlue2010Theme);
    m_menuTheme->addAction(m_actSilver2010Theme);
    m_menuTheme->addAction(m_actBlack2010Theme);
    m_menuView->addMenu(m_menuTheme);
    m_menuView->addSeparator();
    QAction* actStyleDialog = new QAction(tr("Style Dialog..."), this);
    m_menuView->addAction(actStyleDialog);
    connect(actStyleDialog, SIGNAL(triggered()), this, SLOT(createStyleDialog()));

    QMenu* menuHelp = new QMenu(tr("&Help"), this);
    mainMenuBar->addMenu(menuHelp);
    menuHelp->addAction(m_actHelp);
}

void Widgets::createToolbar()
{
    QToolBar* standardBar = new QToolBar(tr("StandardBar"), this);
    standardBar->setAllowedAreas(Qt::AllToolBarAreas);
    standardBar->addAction(m_actNew);
    standardBar->addAction(m_actOpen);
    standardBar->addAction(m_actSave);
    standardBar->addSeparator();
    standardBar->addAction(m_actPrint);
    standardBar->addAction(m_actPrintPreview);
    addToolBar(standardBar);

    QToolBar* customBar = new QToolBar(tr("CustomStyle"), this);
    customBar->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    customBar->setAllowedAreas(Qt::AllToolBarAreas);
    customBar->addAction(m_actDefaultStyle);
    customBar->addAction(m_actOffice2007Style);
    customBar->addAction(m_actRibbonStyle);
    customBar->addSeparator();

    QAction* actTheme = new QAction(QIcon(":/res/customthemes.png"), tr("Custom Themes"), this);
    actTheme->setMenu(m_menuTheme);
    customBar->addAction(actTheme);
    if (QToolButton* button = qobject_cast<QToolButton*>(customBar->widgetForAction(actTheme)))
    {
        button->setPopupMode(QToolButton::InstantPopup);
        button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    }

    addToolBar(customBar);

    QToolBar* explorerBar = new QToolBar(tr("Explorer"), this);
    explorerBar->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    explorerBar->setAllowedAreas(Qt::AllToolBarAreas);
    QAction* act = explorerBar->addAction(QIcon(":/res/back.png"), tr("Back"));
    act->setStatusTip(tr("Back"));
    act = explorerBar->addAction(QIcon(":/res/forward.png"), tr("Forward"));
    act->setStatusTip(tr("Forward"));
    explorerBar->addSeparator();
    act = explorerBar->addAction(QIcon(":/res/up.png"), tr("Up"));
    act->setStatusTip(tr("Up"));
    act = explorerBar->addAction(QIcon(":/res/viewsearch.png"), tr("Search"));
    act->setStatusTip(tr("Search"));
    act = explorerBar->addAction(QIcon(":/res/viewfolders.png"), tr("Folders"));
    act->setStatusTip(tr("Folders"));
    act = explorerBar->addAction(QIcon(":/res/viewhistory.png"), tr("History"));
    act->setStatusTip(tr("History"));
    explorerBar->addSeparator();
    act = explorerBar->addAction(QIcon(":/res/moveto.png"), tr("Move To"));
    act->setStatusTip(tr("Move To"));
    act = explorerBar->addAction(QIcon(":/res/copyto.png"), tr("Copy To"));
    act->setStatusTip(tr("Copy To"));
    act = explorerBar->addAction(QIcon(":/res/delete.png"), tr("Delete"));
    act->setStatusTip(tr("Delete"));
    act = explorerBar->addAction(QIcon(":/res/undo.png"), tr("Undo"));
    act->setStatusTip(tr("Undo"));
    explorerBar->addSeparator();
    act = explorerBar->addAction(QIcon(":/res/views.png"), tr("Views"));
    act->setStatusTip(tr("Views"));
    addToolBar(explorerBar);
}

void Widgets::createDockWindows()
{
    m_styleDock = new QDockWidget(tr("Styles"), this);
    m_styleDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_styleDock->setWidget(new StyleDock());
    addDockWidget(Qt::LeftDockWidgetArea, m_styleDock);

    m_treeDock = new QDockWidget(tr("QTreeView"), this);
    m_treeDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_treeDock->setWidget(new TreeDock());
    addDockWidget(Qt::BottomDockWidgetArea, m_treeDock);

    m_toolBoxDock = new QDockWidget(tr("ToolBox"), this);
    m_toolBoxDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_toolBoxDock->setWidget(new ToolBoxDock());
    addDockWidget(Qt::LeftDockWidgetArea, m_toolBoxDock);

    m_propDock = new QDockWidget(tr("Modules"), this);
    m_propDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_propDock->setWidget(new ModulesDock());
    addDockWidget(Qt::BottomDockWidgetArea, m_propDock);

    m_propDock = new QDockWidget(tr("QTreeWidget"), this);
    m_propDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_propDock->setWidget(new TreeBoxDock());
    addDockWidget(Qt::RightDockWidgetArea, m_propDock);
}

void Widgets::createStyleDialog()
{
    QDialog dlg(this, Qt::MSWindowsFixedSizeDialogHint); 
    Ui::StyleDialog uiDialog;
    uiDialog.setupUi(&dlg);
    // QTreeWidget
    QStringList strings;
    strings.append(tr("Item 1"));
    QTreeWidgetItem* item = new QTreeWidgetItem(strings);
    uiDialog.treeWidget->insertTopLevelItem(0, item);
    strings.clear();
    strings.append(tr("Item 2"));
    uiDialog.treeWidget->insertTopLevelItem(1, new QTreeWidgetItem(item, strings));
    strings.clear();
    strings.append(tr("Item 3"));
    uiDialog.treeWidget->insertTopLevelItem(2, new QTreeWidgetItem(item, strings));
    strings.clear();
    strings.append(tr("Item 4"));
    uiDialog.treeWidget->insertTopLevelItem(3, new QTreeWidgetItem(item, strings));
    strings.clear();
    strings.append(tr("Item 5"));
    uiDialog.treeWidget->insertTopLevelItem(4, new QTreeWidgetItem(item, strings));
    strings.clear();
    strings.append(tr("Item 6"));
    uiDialog.treeWidget->insertTopLevelItem(6, new QTreeWidgetItem(item, strings));
    uiDialog.treeWidget->expandItem(item);

    // QtableView
    uiDialog.tableWidget->setColumnCount(2);
    uiDialog.tableWidget->setRowCount(6);
    // uiDialog.tableWidget->setShowGrid(false);
    uiDialog.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    uiDialog.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    uiDialog.tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    uiDialog.tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Col 1")));
    uiDialog.tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Col 2")));
    int height = uiDialog.tableWidget->horizontalHeader()->sizeHint().height();
    uiDialog.tableWidget->setRowHeight(0, height);
    uiDialog.tableWidget->setRowHeight(1, height);
    uiDialog.tableWidget->setRowHeight(2, height);
    uiDialog.tableWidget->setRowHeight(3, height);
    uiDialog.tableWidget->setRowHeight(4, height);
    uiDialog.tableWidget->setRowHeight(5, height);
    // QListWidget
    uiDialog.listWidget->insertItem(0, tr("Item 1"));
    uiDialog.listWidget->insertItem(1, tr("Item 2"));
    uiDialog.listWidget->insertItem(2, tr("Item 3"));
    uiDialog.listWidget->insertItem(3, tr("Item 4"));
    uiDialog.listWidget->insertItem(4, tr("Item 5"));
    uiDialog.listWidget->insertItem(5, tr("Item 6"));
    uiDialog.listWidget->setCurrentRow(3);

    uiDialog.comboBox->addItem(tr("Normal"));
    uiDialog.comboBox_2->addItem(tr("Disabled"));
    uiDialog.lineEdit->setText(tr("Normal"));

    dlg.exec();
}

QTextEdit* Widgets::createMdiChild()
{
    static int sequenceNumber = 1;

    QString curFile = tr("document%1.txt").arg(sequenceNumber++);

    QTextEdit* child = new QTextEdit;
    child->setWindowIcon(QIcon(":/res/new.png"));
    child->setWindowTitle(curFile + "[*]");

    QMdiSubWindow* subWindow = m_mdiArea->addSubWindow(child);
    subWindow->setWindowState(Qt::WindowMaximized);
//    subWindow->resize( m_mdiArea->width(), m_mdiArea->height());
    child->show();
    return child;
}

void Widgets::triggeredStyles(QAction* action)
{
    updateStyle(action);
}

void Widgets::triggeredThemes(QAction* action)
{
    updateTheme(action);
}

void Widgets::updateStyle(QAction* act)
{
    if (m_actDefaultStyle == act)
    {
        if (!m_styleName.isEmpty())
        {
            qApp->setStyle(QStyleFactory::create(m_styleName));
            m_groupThemesActions->setEnabled(false);
            m_officeStyle = Q_NULL;
        }
    }
    else if (m_actOffice2007Style == act)
    {
        m_officeStyle = new Qtitan::OfficeStyle();
        m_officeStyle->setDialogsIgnored(false);
        qApp->setStyle(m_officeStyle);
        m_groupThemesActions->setEnabled(true);
        updateTheme(m_groupThemesActions->checkedAction());
    }
    else if (m_actRibbonStyle == act)
    {
        m_officeStyle = new Qtitan::RibbonStyle();
        m_officeStyle->setDialogsIgnored(false);
        qApp->setStyle(m_officeStyle);
        m_groupThemesActions->setEnabled(true);
        updateTheme(m_groupThemesActions->checkedAction());
    }

    act->setChecked(true);
}

void Widgets::updateTheme(QAction* act)
{
    if (!act)
        return;

    if (m_officeStyle)
    {
        Qtitan::OfficeStyle::Theme themeId = Qtitan::OfficeStyle::Office2007Blue;
        if (act == m_actBlueTheme)
            themeId = Qtitan::OfficeStyle::Office2007Blue;
        else if (act == m_actBlackTheme)
            themeId = Qtitan::OfficeStyle::Office2007Black;
        else if (act == m_actSilverTheme)
            themeId = Qtitan::OfficeStyle::Office2007Silver;
        else if (act == m_actAquaTheme)
            themeId = Qtitan::OfficeStyle::Office2007Aqua;
        else if (act == m_actScenicTheme)
            themeId = Qtitan::OfficeStyle::Windows7Scenic;
        else if (act == m_actBlue2010Theme)
            themeId = Qtitan::OfficeStyle::Office2010Blue;
        else if (act == m_actSilver2010Theme)
            themeId = Qtitan::OfficeStyle::Office2010Silver;
        else if (act == m_actBlack2010Theme)
            themeId = Qtitan::OfficeStyle::Office2010Black;

        act->setChecked(true);
        m_officeStyle->setTheme(themeId);
    }
}

void Widgets::readSettings()
{
    QSettings settings("MainWindow", "Qtitan Widgets Sample");

    QRect geom = QApplication::desktop()->availableGeometry();
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(2 * geom.width() / 3, 2 * geom.height() / 3)).toSize();
    resize(size);
    move(pos);
}

void Widgets::writeSettings()
{
    QSettings settings("MainWindow", "Qtitan Widgets Sample");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

void Widgets::about()
{
    Qtitan::AboutDialog::show(this, tr("About Qtitan Widgets"), 
        tr("OfficeStyle"), QLatin1String(QTN_VERSION_OFFICE_STR));
}



