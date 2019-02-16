
#include <QApplication>
#include <QPrinter>
#include <QPrintDialog>
#include <QFileDialog>
#include <QDesktopWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QProgressBar>
#include <QLineEdit>
#include <QRadioButton>
#include <QScrollBar>
#include <QFontComboBox>
#include <QAction>
#include <QMenu>
#include <QLabel>
#include <QSpinBox>
#include <QMessageBox>
#include <QButtonGroup>
#include <QPushButton>
#include <QSettings>
#include <QPainter>
#include <QStyleOption>

#include <QtnOfficeDefines.h>
#include <QtnRibbonQuickAccessBar.h>
#include <QtnRibbonBackstageView.h>

#include "aboutdialog.h"
#include "mainwindow.h"
#include "backstagepages.h"

#define Prop_ContextColor   "ContextColor"

/* CentralWidget */
class WorkspaceWidget : public QWidget
{
public:
    WorkspaceWidget() 
    {
        setAttribute(Qt::WA_MouseTracking);
        setAutoFillBackground(true);
        QPalette pal = palette();
        pal.setColor(QPalette::Background, QColor(255, 255, 255));
        setPalette(pal);
    }
protected:
    virtual void mousePressEvent(QMouseEvent* event)
    {
        Q_UNUSED(event);
        setFocus();
    }
    virtual void paintEvent(QPaintEvent* event)
    {
        Q_UNUSED(event);
        QPainter p(this);
        QStyleOption opt;
        opt.init(this);
        style()->drawPrimitive((QStyle::PrimitiveElement)CommonStyle::PE_Workspace, &opt, &p, this);
    }
};

/* MainWindow */
MainWindow::MainWindow(QWidget* parent)
    : Qtitan::RibbonMainWindow(parent)
{
    m_ribbonStyle = qobject_cast<Qtitan::RibbonStyle*>(qApp->style());

    setWindowTitle(tr("Qtitan Ribbon Backstage Sample"));

    setCentralWidget(new WorkspaceWidget);

    createBackstage();
    createQuickAccessBar();
    createRibbon();
    statusBar();

    QAction* actionAbout = ribbonBar()->addAction(QIcon(":/shared/res/about.png"), "About", Qt::ToolButtonIconOnly);
    actionAbout->setToolTip(tr("Display program<br />information, version number and copyright"));
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    createOptions();

    m_actionRibbonMinimize = ribbonBar()->addAction(QIcon(":/shared/res/ribbonMinimize.png"), "Minimize the Ribbon", Qt::ToolButtonIconOnly);
    m_actionRibbonMinimize->setStatusTip(tr("Show only the tab names on the Ribbon"));
    m_actionRibbonMinimize->setShortcut(tr("Ctrl+F1"));
    connect(m_actionRibbonMinimize, SIGNAL(triggered()), this, SLOT(maximizeToggle()));
    connect(ribbonBar(), SIGNAL(minimizationChanged(bool)), this, SLOT(minimizationChanged(bool)));

    QRect geom = QApplication::desktop()->availableGeometry();
    resize(2 * geom.width() / 3, 2 * geom.height() / 3);

    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    settings.setValue("recentFileList", files);
    emit updateRecentFileActions(files);
}

MainWindow::~MainWindow()
{
}

void MainWindow::createBackstage()
{
    QIcon iconLogo;
    iconLogo.addPixmap(QPixmap(":/res/file.png"));
    iconLogo.addPixmap(QPixmap(":/shared/res/qtitanlogo32x32.png"));
    if (QAction* actionFile = ribbonBar()->addSystemButton(iconLogo, tr("&File")))
    {
        actionFile->setToolTip(tr("Click here to see everything<br />you can do with your<br />document"));

        Qtitan::RibbonBackstageView* backstage = new Qtitan::RibbonBackstageView(ribbonBar());
        connect(backstage, SIGNAL(aboutToShow()), this, SLOT(showBackstage()));
        connect(backstage, SIGNAL(aboutToHide()), this, SLOT(hideBackstage()));

        QAction* saveFile = backstage->addAction(QIcon(":/res/smallSave.png"), tr("&Save"));
        saveFile->setShortcut(tr("Ctrl+S"));
        saveFile->setToolTip(tr("Save"));
        saveFile->setStatusTip(tr("Save the active document"));
        connect(saveFile, SIGNAL(triggered()), this, SLOT(save()));

        QAction* saveAsFile = backstage->addAction(QIcon(":/res/smallSaveAs.png"), tr("Save &As"));
        saveAsFile->setToolTip(tr("Save As"));
        saveAsFile->setStatusTip(tr("Save the active document with a new name"));
        connect(saveAsFile, SIGNAL(triggered()), this, SLOT(save()));

        QAction* openFile = backstage->addAction(QIcon(":/res/smallOpen.png"), tr("&Open"));
        openFile->setShortcut(tr("Ctrl+O"));
        openFile->setToolTip(tr("Open"));
        openFile->setStatusTip(tr("Open an existing document"));
        connect(openFile, SIGNAL(triggered()), this, SLOT(open()));

        BackstagePageInfo* infoPage = new BackstagePageInfo(this);
        backstage->addPage(infoPage);

        BackstagePageSave* savePage = new BackstagePageSave(this);
        backstage->addPage(savePage);

        BackstagePageAbout* aboutPage = new BackstagePageAbout(this);
        backstage->addPage(aboutPage);

        QAction* actOptions = backstage->addAction(QIcon(":/res/smallOptions.png"), tr("Options"));
        connect(actOptions, SIGNAL(triggered()), this, SLOT(pressButton()));

        QAction* actExit = backstage->addAction(QIcon(":/res/smallClose.png"), tr("Exit"));
        connect(actExit, SIGNAL(triggered()), this, SLOT(close()));

        if (RibbonSystemButton* sysButton = ribbonBar()->getSystemButton())
            sysButton->setBackstage(backstage);
    }
}

void MainWindow::createQuickAccessBar()
{
    if (Qtitan::RibbonQuickAccessBar* quickAccessBar = ribbonBar()->getQuickAccessBar())
    {
        QAction* action = quickAccessBar->actionCustomizeButton();
        action->setToolTip(tr("Customize Quick Access Bar"));

        QAction* smallButton = quickAccessBar->addAction(QIcon(":/res/smallNew.png"), tr("New"));
        smallButton->setToolTip(tr("Create a new document"));
        connect(smallButton, SIGNAL(triggered()), this, SLOT(pressButton()));
        quickAccessBar->setActionVisible(smallButton, false);

        smallButton = quickAccessBar->addAction(QIcon(":/res/smallOpen.png"), tr("Open"));
        smallButton->setToolTip(tr("Open an existing document"));
        connect(smallButton, SIGNAL(triggered()), this, SLOT(pressButton()));
        quickAccessBar->setActionVisible(smallButton, false);

        smallButton = quickAccessBar->addAction(QIcon(":/res/smallSave.png"), tr("Save"));
        smallButton->setToolTip(tr("Save the active document"));
        connect(smallButton, SIGNAL(triggered()), this, SLOT(pressButton()));

        smallButton = quickAccessBar->addAction(QIcon(":/res/smallUndo.png"), tr("&Undo"));
        smallButton->setShortcut(QKeySequence::Undo);
        smallButton->setEnabled(false);

        smallButton = quickAccessBar->addAction(QIcon(":/res/smallRedo.png"), tr("&Redo"));
        smallButton->setShortcut(QKeySequence::Redo);
        smallButton->setEnabled(false);

        ribbonBar()->showQuickAccess(true);
    }
}

void MainWindow::createRibbon()
{
    if (Qtitan::RibbonPage* pageButtons = ribbonBar()->addPage(tr("&Buttons")))
    {
        QMenu* splitPopup = new QMenu(this);
        splitPopup->addAction(tr("Option 1"));
        splitPopup->addAction(tr("Option 2"));
        splitPopup->addAction(tr("Option 3"));

        if (Qtitan::RibbonGroup* groupClipboard = pageButtons->addGroup(tr("Clipboard"))) 
        {
            QMenu* editPaste = new QMenu(this);
            QAction* actionPaste1 = editPaste->addAction(QIcon(":/res/smallClipboard.png"), tr("Paste"));
            actionPaste1->setPriority(QAction::LowPriority);
            actionPaste1->setShortcut(QKeySequence::Paste);

            QAction* actionPaste2 = groupClipboard->addAction(QIcon(":/res/clipboard.png"), 
                tr("&Paste"), Qt::ToolButtonTextUnderIcon, editPaste);
            actionPaste2->setPriority(QAction::LowPriority);
            actionPaste2->setShortcut(QKeySequence::Paste);

            QAction* actionCut = groupClipboard->addAction(QIcon(":/res/smallCut.png"), 
                tr("&Cut"), Qt::ToolButtonTextBesideIcon);
            actionCut->setShortcut(QKeySequence::Cut);

            QAction* actionCopy = groupClipboard->addAction(QIcon(":/res/smallCopy.png"), 
                tr("&Copy"), Qt::ToolButtonTextBesideIcon);
            actionCopy->setShortcut(QKeySequence::Copy);

            groupClipboard->addAction(QIcon(":/res/smallFormatPainter.png"), 
                tr("F&ormat Pointer"), Qt::ToolButtonTextBesideIcon);

            groupClipboard->setEnabled(false);
        }

        if (Qtitan::RibbonGroup* groupView = pageButtons->addGroup(tr("View"))) 
        {
            QCheckBox* checkStatusBar = new QCheckBox(tr("Status Bar"));
            checkStatusBar->setToolTip(tr("CheckBox 1"));
            checkStatusBar->setCheckState(Qt::Checked);
            groupView->addWidget(checkStatusBar);
            connect(checkStatusBar, SIGNAL(stateChanged(int)), this, SLOT(hideStatusBar(int)));
        }
    }

    if (Qtitan::RibbonPage* pageWindow = ribbonBar()->addPage(tr("&Window")))
    {
        Q_UNUSED(pageWindow);
    }

    if (Qtitan::RibbonPage* pageContext = ribbonBar()->addPage(tr("&Context Tab")))
    {
        pageContext->setContextPage(Qtitan::ContextColorGreen);
    }

    ribbonBar()->setFrameThemeEnabled();
}

void MainWindow::createOptions()
{
    Qtitan::RibbonStyle::Theme styleId = Qtitan::RibbonStyle::Office2007Blue;
    if (m_ribbonStyle)
        styleId = m_ribbonStyle->getTheme();

    m_menuOptions = ribbonBar()->addMenu(tr("Options"));
    QAction* actionStyle = m_menuOptions->addAction(tr("Style"));

    QMenu* menuStyle = new QMenu(this);
    QActionGroup* styleActions = new QActionGroup(this);

    QAction* actionBlue = menuStyle->addAction(tr("Office 2007 Blue"));
    actionBlue->setCheckable(true);
    actionBlue->setChecked(styleId == Qtitan::RibbonStyle::Office2007Blue);
    actionBlue->setObjectName("OS_OFFICE2007BLUE");

    QAction* actionBlack = menuStyle->addAction(tr("Office 2007 Black"));
    actionBlack->setObjectName("OS_OFFICE2007BLACK");
    actionBlack->setCheckable(true);
    actionBlack->setChecked(styleId == Qtitan::RibbonStyle::Office2007Black);

    QAction* actionSilver = menuStyle->addAction(tr("Office 2007 Silver"));
    actionSilver->setObjectName("OS_OFFICE2007SILVER");
    actionSilver->setCheckable(true);
    actionSilver->setChecked(styleId == Qtitan::RibbonStyle::Office2007Silver);

    QAction* actionAqua = menuStyle->addAction(tr("Office 2007 Aqua"));
    actionAqua->setObjectName("OS_OFFICE2007AQUA");
    actionAqua->setCheckable(true);
    actionAqua->setChecked(styleId == Qtitan::RibbonStyle::Office2007Aqua);

    QAction* actionScenic = menuStyle->addAction(tr("Windows 7 Scenic"));
    actionScenic->setObjectName("OS_WINDOWS7SCENIC");
    actionScenic->setCheckable(true);
    actionScenic->setChecked(styleId == Qtitan::RibbonStyle::Windows7Scenic);

    QAction* action2010Blue = menuStyle->addAction(tr("Office 2010 Blue"));
    action2010Blue->setObjectName("OS_OFFICE2010BLUE");
    action2010Blue->setCheckable(true);
    action2010Blue->setChecked(styleId == Qtitan::RibbonStyle::Office2010Blue);

    QAction* action2010Silver = menuStyle->addAction(tr("Office 2010 Silver"));
    action2010Silver->setObjectName("OS_OFFICE2010SILVER");
    action2010Silver->setCheckable(true);
    action2010Silver->setChecked(styleId == Qtitan::RibbonStyle::Office2010Silver);

    QAction* action2010Black = menuStyle->addAction(tr("Office 2010 Black"));
    action2010Black->setObjectName("OS_OFFICE2010BLACK");
    action2010Black->setCheckable(true);
    action2010Black->setChecked(styleId == Qtitan::RibbonStyle::Office2010Black);

    styleActions->addAction(actionBlue);
    styleActions->addAction(actionBlack);
    styleActions->addAction(actionSilver);
    styleActions->addAction(actionAqua);
    styleActions->addAction(actionScenic);
    styleActions->addAction(action2010Blue);
    styleActions->addAction(action2010Silver);
    styleActions->addAction(action2010Black);

    actionStyle->setMenu(menuStyle);
    connect(styleActions, SIGNAL(triggered(QAction*)), this, SLOT(options(QAction*)));

    m_menuOptions->addSeparator();
    QAction* actionDPI_100_percent = m_menuOptions->addAction(tr("DPI Aware (new)"));
    actionDPI_100_percent->setCheckable(true);
    actionDPI_100_percent->setChecked(true);
    connect(actionDPI_100_percent, SIGNAL(toggled(bool)), this, SLOT(setDPIToggled(bool)));

    m_menuOptions->addSeparator();
    QAction* actionCusomize = m_menuOptions->addAction(tr("Cusomize..."));
    actionCusomize->setEnabled(false);
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open"));
    if (!fileName.isEmpty()) 
    {
        setCurrentFile(fileName);
        statusBar()->showMessage(tr("File loaded"), 2000);
    }
}

bool MainWindow::save()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),tr("Document"), QLatin1Char('*') + QLatin1String(".txt"));
    if (!fileName.isEmpty())
    {
        setCurrentFile(fileName);
        statusBar()->showMessage(tr("File saved"), 2000);
        return true;
    }
    return false;
}

void MainWindow::openRecentFile(const QString& file)
{
    QMessageBox messageBox(QMessageBox::Information, windowTitle(), 
        QLatin1String(""), QMessageBox::Ok, this);
    messageBox.setInformativeText(QLatin1String("Open Recent File.") + QLatin1String(" ") + file);
    messageBox.exec();
}

#ifndef QT_NO_PRINTER
void MainWindow::print()
{
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Accepted)
    {
    }
}

void MainWindow::printSetup()
{
}
#endif

void MainWindow::about()
{
    Qtitan::AboutDialog::show(this, tr("About Qtitan Ribbon Controls Sample"), 
        tr("QtitanRibbon"), QLatin1String(QTN_VERSION_RIBBON_STR));
}

void MainWindow::options(QAction* action)
{
    if (m_ribbonStyle)
    {
        Qtitan::RibbonStyle::Theme styleId = Qtitan::RibbonStyle::Office2007Blue;
        if (action->objectName() == tr("OS_OFFICE2007BLACK"))
            styleId = Qtitan::RibbonStyle::Office2007Black;
        else if (action->objectName() == tr("OS_OFFICE2007SILVER"))
            styleId = Qtitan::RibbonStyle::Office2007Silver;
        else if (action->objectName() == tr("OS_OFFICE2007AQUA"))
            styleId = Qtitan::RibbonStyle::Office2007Aqua;
        else if (action->objectName() == tr("OS_WINDOWS7SCENIC"))
            styleId = Qtitan::OfficeStyle::Windows7Scenic;
        else if (action->objectName() == tr("OS_OFFICE2010BLUE"))
            styleId = Qtitan::OfficeStyle::Office2010Blue;
        else if (action->objectName() == tr("OS_OFFICE2010SILVER"))
            styleId = Qtitan::OfficeStyle::Office2010Silver;
        else if (action->objectName() == tr("OS_OFFICE2010BLACK"))
            styleId = Qtitan::OfficeStyle::Office2010Black;

        if (QToolButton* button = ribbonBar()->getSystemButton())
        {
            if (styleId == Qtitan::OfficeStyle::Windows7Scenic || 
                styleId == Qtitan::OfficeStyle::Office2010Blue ||
                styleId == Qtitan::OfficeStyle::Office2010Silver ||
                styleId == Qtitan::OfficeStyle::Office2010Black)
                button->setToolButtonStyle(Qt::ToolButtonTextOnly);
            else
                button->setToolButtonStyle(Qt::ToolButtonFollowStyle);
        }

        m_ribbonStyle->setTheme(styleId);
    }
}

void MainWindow::hideStatusBar(int state)
{
    statusBar()->setVisible(state == Qt::Checked);
}

void MainWindow::pressButton()
{
    QMessageBox messageBox(QMessageBox::Information, windowTitle(), 
        QLatin1String(""), QMessageBox::Ok, this);
    messageBox.setInformativeText(QLatin1String("Press button."));
    messageBox.exec();
}

void MainWindow::maximizeToggle()
{
    ribbonBar()->setMinimized(!ribbonBar()->isMinimized());
}

void MainWindow::minimizationChanged(bool minimized)
{
    m_actionRibbonMinimize->setChecked(minimized);
    m_actionRibbonMinimize->setIcon(minimized ? QIcon(":/shared/res/ribbonMaximize.png") :  QIcon(":/shared/res/ribbonMinimize.png"));
}

void MainWindow::showBackstage()
{
    if (Qtitan::RibbonQuickAccessBar* quickAccessBar = ribbonBar()->getQuickAccessBar())
        quickAccessBar->setEnabled(false);

    m_actionRibbonMinimize->setEnabled(false);
    m_menuOptions->setEnabled(false);
}

void MainWindow::hideBackstage()
{
    if (Qtitan::RibbonQuickAccessBar* quickAccessBar = ribbonBar()->getQuickAccessBar())
        quickAccessBar->setEnabled(true);

    m_actionRibbonMinimize->setEnabled(true);
    m_menuOptions->setEnabled(true);
}

void MainWindow::setDPIToggled(bool on)
{
    m_ribbonStyle->setDPIAware(on);
}

void MainWindow::setCurrentFile(const QString& fileName)
{
    QString curFile = fileName;
    setWindowFilePath(curFile);

    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);

    while (files.size() > 9)
        files.removeLast();

    settings.setValue("recentFileList", files);

    emit updateRecentFileActions(files);
}
